/*
 * nghttp2 - HTTP/2 C Library
 *
 * Copyright (c) 2013 Tatsuhiro Tsujikawa
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
 * LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
 * OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 * WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */
#include "nghttp.h"

#include <sys/stat.h>
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif // HAVE_UNISTD_H
#ifdef HAVE_FCNTL_H
#include <fcntl.h>
#endif // HAVE_FCNTL_H
#ifdef HAVE_NETINET_IN_H
#include <netinet/in.h>
#endif // HAVE_NETINET_IN_H
#include <netinet/tcp.h>
#include <getopt.h>
#include <chrono>

#include <cassert>
#include <cstdio>
#include <cerrno>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <tuple>
#include <vector>
#include <ctime>
#include <openssl/err.h>
#include <math.h>
#include <utility>
#ifdef HAVE_JANSSON
#include <jansson.h>
#endif // HAVE_JANSSON

#include "app_helper.h"
#include "HtmlParser.h"
#include "util.h"
#include "base64.h"
#include "ssl.h"
#include "template.h"
#include <fstream>
#include <boost/algorithm/string.hpp>
#include <boost/bind.hpp>
// #include <boost/asio.hpp>
// #include <boost/date_time/posix_time/posix_time.hpp>
//#include <chrono>
//#include <thread>
//#include <mutex>
//#include <boost/algorithm/string.hpp>
//#include <boost/bind.hpp>
//#include <nghttp2/asio_http2_server.h>
#include <netdb.h>

#include <netinet/in.h>

#include "duc_metric.h"
//#include "common.h"

#ifndef O_BINARY
#define O_BINARY (0)
#endif // O_BINARY
using namespace std;
// using namespace boost::posix_time;
int a = 0;
std::vector<int> hung_rate_set = {200,  250,  300,  400,  500,  600,
                                  700,  900,  1200, 1500, 2000, 2500,
                                  3000, 4000, 5000, 6000};

const char *confFile = "metadata/metadata.cfg";
Manifest meta(confFile);

/* metrics */
Metric metric(meta.NO_SEG, meta.NO_FACE, meta.EL_VER_NUM + 1);

/* Buffer */
// Buffer buff(meta.NO_SEG, NO_TILE);

/* head traces */
Headtrace htrace(meta.htrace.c_str(), meta.VLEN *meta.FPS, meta.FPS);

/* visible mask */
VisibleMask vmask(meta.vmask.c_str(), meta.NO_TILE);

/* Player */
Player player(meta.BASE_BUFF_THRES, meta.VLEN, meta.SD, meta.NO_TILE, &vmask,
              &htrace, meta.ALG, meta.base_layer_br, meta.EL_VER_NUM,
              meta.enha_layer_br);
thread player_thread(&Player::start_playback, &player);

/* download status */
int req_s_num = 0;   /* number of request streams */
int fin_s_num = 0;   /* number of finsihed streams */
int BL_seg_id = 0;   /* base layer segment id */
int EL_seg_id = 0;   /* enhancement layer segment id */
int play_seg_id = 0; /* playing segment id */
int INI_STREAM_ID =
    13; /* First request stream id -> adaptation start *after* this stream */
int cur_req_stream_id; /* current request stream id */
int iter_id = 0;       /* downloading interation */
bool INI_BUFFERING = true;
bool RE_BUFFERRING = false;
int LAST_ITER_MASK = 1;
int max_layerId = 4, min_layerId = 1;

/* cancelled stream */
vector<int> cancelled_request_stream;

/* output logs */
FILE *log_stream, *log_iter;

namespace nghttp2 {

// The anchor stream nodes when --no-dep is not used.  The stream ID =
// 1 is excluded since it is used as first stream in upgrade case.  We
// follows the same dependency anchor nodes as Firefox does.
struct Anchor {
  int32_t stream_id;
  // stream ID this anchor depends on
  int32_t dep_stream_id;
  // .. with this weight.
  int32_t weight;
};

// This is index into anchors.  Firefox uses ANCHOR_FOLLOWERS for html
// file.
enum {
  ANCHOR_LEADERS,
  ANCHOR_UNBLOCKED,
  ANCHOR_BACKGROUND,
  ANCHOR_SPECULATIVE,
  ANCHOR_FOLLOWERS,
};

namespace {
constexpr auto anchors = std::array<Anchor, 5>{{
    {3, 0, 201},
    {5, 0, 101},
    {7, 0, 1},
    {9, 7, 1},
    {11, 3, 1},
}};
} // namespace

Config::Config()
    : header_table_size(-1),
      min_header_table_size(std::numeric_limits<uint32_t>::max()),
      encoder_header_table_size(-1),
      padding(0),
      max_concurrent_streams(100),
      peer_max_concurrent_streams(100),
      multiply(1),
      timeout(0.),
      window_bits(-1),
      connection_window_bits(-1),
      verbose(0),
      null_out(false),
      remote_name(false),
      get_assets(false),
      stat(false),
      upgrade(false),
      continuation(false),
      no_content_length(false),
      no_dep(false),
      hexdump(false),
      no_push(false),
      expect_continue(false) {
  nghttp2_option_new(&http2_option);
  nghttp2_option_set_peer_max_concurrent_streams(http2_option,
                                                 peer_max_concurrent_streams);
  nghttp2_option_set_builtin_recv_extension_type(http2_option, NGHTTP2_ALTSVC);
}

Config::~Config() { nghttp2_option_del(http2_option); }

namespace {
Config config;
} // namespace

namespace {
void print_protocol_nego_error() {
  std::cerr << "[ERROR] HTTP/2 protocol was not selected."
            << " (nghttp2 expects " << NGHTTP2_PROTO_VERSION_ID << ")"
            << std::endl;
}
} // namespace

namespace {
std::string strip_fragment(const char *raw_uri) {
  const char *end;
  for (end = raw_uri; *end && *end != '#'; ++end)
    ;
  size_t len = end - raw_uri;
  return std::string(raw_uri, len);
}
} // namespace

Request::Request(const std::string &uri, const http_parser_url &u,
                 const nghttp2_data_provider *data_prd, int64_t data_length,
                 const nghttp2_priority_spec &pri_spec, int level)
    : uri(uri),
      u(u),
      pri_spec(pri_spec),
      data_length(data_length),
      data_offset(0),
      response_len(0),
      inflater(nullptr),
      html_parser(nullptr),
      data_prd(data_prd),
      header_buffer_size(0),
      stream_id(-1),
      status(0),
      level(level),
      expect_final_response(false) {
  http2::init_hdidx(res_hdidx);
  http2::init_hdidx(req_hdidx);
}

Request::~Request() {
  nghttp2_gzip_inflate_del(inflater);
  delete html_parser;
}

void Request::init_inflater() {
  int rv;
  rv = nghttp2_gzip_inflate_new(&inflater);
  assert(rv == 0);
}

void Request::init_html_parser() { html_parser = new HtmlParser(uri); }

int Request::update_html_parser(const uint8_t *data, size_t len, int fin) {
  if (!html_parser) {
    return 0;
  }
  return html_parser->parse_chunk(reinterpret_cast<const char *>(data), len,
                                  fin);
}

std::string Request::make_reqpath() const {
  std::string path = util::has_uri_field(u, UF_PATH)
                         ? util::get_uri_field(uri.c_str(), u, UF_PATH).str()
                         : "/";
  if (util::has_uri_field(u, UF_QUERY)) {
    path += '?';
    path.append(uri.c_str() + u.field_data[UF_QUERY].off,
                u.field_data[UF_QUERY].len);
  }
  return path;
}

namespace {
// Perform special handling |host| if it is IPv6 literal and includes
// zone ID per RFC 6874.
std::string decode_host(const StringRef &host) {
  auto zone_start = std::find(std::begin(host), std::end(host), '%');
  if (zone_start == std::end(host) ||
      !util::ipv6_numeric_addr(
          std::string(std::begin(host), zone_start).c_str())) {
    return host.str();
  }
  // case: ::1%
  if (zone_start + 1 == std::end(host)) {
    return StringRef{host.c_str(), host.size() - 1}.str();
  }
  // case: ::1%12 or ::1%1
  if (zone_start + 3 >= std::end(host)) {
    return host.str();
  }
  // If we see "%25", followed by more characters, then decode %25 as
  // '%'.
  auto zone_id_src = (*(zone_start + 1) == '2' && *(zone_start + 2) == '5')
                         ? zone_start + 3
                         : zone_start + 1;
  auto zone_id = util::percent_decode(zone_id_src, std::end(host));
  auto res = std::string(std::begin(host), zone_start + 1);
  res += zone_id;
  return res;
}
} // namespace

namespace {
nghttp2_priority_spec resolve_dep(int res_type) {
  nghttp2_priority_spec pri_spec;

  if (config.no_dep) {
    nghttp2_priority_spec_default_init(&pri_spec);

    return pri_spec;
  }

  int32_t anchor_id;
  int32_t weight;
  switch (res_type) {
  case REQ_CSS:
  case REQ_JS:
    anchor_id = anchors[ANCHOR_LEADERS].stream_id;
    weight = 32;
    break;
  case REQ_UNBLOCK_JS:
    anchor_id = anchors[ANCHOR_UNBLOCKED].stream_id;
    weight = 32;
    break;
  case REQ_IMG:
    anchor_id = anchors[ANCHOR_FOLLOWERS].stream_id;
    weight = 12;
    break;
  default:
    anchor_id = anchors[ANCHOR_FOLLOWERS].stream_id;
    weight = 32;
  }

  nghttp2_priority_spec_init(&pri_spec, anchor_id, weight, 0);
  return pri_spec;
}
} // namespace

bool Request::is_ipv6_literal_addr() const {
  if (util::has_uri_field(u, UF_HOST)) {
    return memchr(uri.c_str() + u.field_data[UF_HOST].off, ':',
                  u.field_data[UF_HOST].len);
  } else {
    return false;
  }
}

Headers::value_type *Request::get_res_header(int32_t token) {
  auto idx = res_hdidx[token];
  if (idx == -1) {
    return nullptr;
  }
  return &res_nva[idx];
}

Headers::value_type *Request::get_req_header(int32_t token) {
  auto idx = req_hdidx[token];
  if (idx == -1) {
    return nullptr;
  }
  return &req_nva[idx];
}

void Request::record_request_start_time() {
  timing.state = RequestState::ON_REQUEST;
  timing.request_start_time = get_time();
}

void Request::record_response_start_time() {
  timing.state = RequestState::ON_RESPONSE;
  timing.response_start_time = get_time();
}

void Request::record_response_end_time() {
  timing.state = RequestState::ON_COMPLETE;
  timing.response_end_time = get_time();
}

namespace {
void continue_timeout_cb(struct ev_loop *loop, ev_timer *w, int revents) {
  auto client = static_cast<HttpClient *>(ev_userdata(loop));
  auto req = static_cast<Request *>(w->data);
  int error;

  error = nghttp2_submit_data(client->session, NGHTTP2_FLAG_END_STREAM,
                              req->stream_id, req->data_prd);

  if (error) {
    std::cerr << "[ERROR] nghttp2_submit_data() returned error: "
              << nghttp2_strerror(error) << std::endl;
    nghttp2_submit_rst_stream(client->session, NGHTTP2_FLAG_NONE,
                              req->stream_id, NGHTTP2_INTERNAL_ERROR);
  }

  client->signal_write();
}
} // namespace

ContinueTimer::ContinueTimer(struct ev_loop *loop, Request *req) : loop(loop) {
  ev_timer_init(&timer, continue_timeout_cb, 1., 0.);
  timer.data = req;
}

ContinueTimer::~ContinueTimer() { stop(); }

void ContinueTimer::start() { ev_timer_start(loop, &timer); }

void ContinueTimer::stop() { ev_timer_stop(loop, &timer); }

void ContinueTimer::dispatch_continue() {
  // Only dispatch the timeout callback if it hasn't already been called.
  if (ev_is_active(&timer)) {
    ev_feed_event(loop, &timer, 0);
  }
}

namespace {
int htp_msg_begincb(http_parser *htp) {
  if (config.verbose) {
    print_timer();
    std::cout << " HTTP Upgrade response" << std::endl;
  }
  return 0;
}
} // namespace

namespace {
int htp_statuscb(http_parser *htp, const char *at, size_t length) {
  auto client = static_cast<HttpClient *>(htp->data);
  client->upgrade_response_status_code = htp->status_code;
  return 0;
}
} // namespace

namespace {
int htp_msg_completecb(http_parser *htp) {
  auto client = static_cast<HttpClient *>(htp->data);
  client->upgrade_response_complete = true;
  return 0;
}
} // namespace

namespace {
http_parser_settings htp_hooks = {
    htp_msg_begincb,   // http_cb      on_message_begin;
    nullptr,           // http_data_cb on_url;
    htp_statuscb,      // http_data_cb on_status;
    nullptr,           // http_data_cb on_header_field;
    nullptr,           // http_data_cb on_header_value;
    nullptr,           // http_cb      on_headers_complete;
    nullptr,           // http_data_cb on_body;
    htp_msg_completecb // http_cb      on_message_complete;
};
} // namespace

namespace {
int submit_request(HttpClient *client, const Headers &headers, Request *req) {
  auto path = req->make_reqpath();
  auto scheme = util::get_uri_field(req->uri.c_str(), req->u, UF_SCHEMA);
  auto build_headers = Headers{{":method", req->data_prd ? "POST" : "GET"},
                               {":path", path},
                               {":scheme", scheme.str()},
                               {":authority", client->hostport},
                               {"accept", "*/*"},
                               {"accept-encoding", "gzip, deflate"},
                               {"user-agent", "nghttp2/" NGHTTP2_VERSION}};
  bool expect_continue = false;

  if (config.continuation) {
    for (size_t i = 0; i < 6; ++i) {
      build_headers.emplace_back("continuation-test-" + util::utos(i + 1),
                                 std::string(4_k, '-'));
    }
  }

  auto num_initial_headers = build_headers.size();

  if (req->data_prd) {
    if (!config.no_content_length) {
      build_headers.emplace_back("content-length",
                                 util::utos(req->data_length));
    }
    if (config.expect_continue) {
      expect_continue = true;
      build_headers.emplace_back("expect", "100-continue");
    }
  }

  for (auto &kv : headers) {
    size_t i;
    for (i = 0; i < num_initial_headers; ++i) {
      if (kv.name == build_headers[i].name) {
        build_headers[i].value = kv.value;
        break;
      }
    }
    if (i < num_initial_headers) {
      continue;
    }

    build_headers.emplace_back(kv.name, kv.value, kv.no_index);
  }

  auto nva = std::vector<nghttp2_nv>();
  nva.reserve(build_headers.size());

  for (auto &kv : build_headers) {
    nva.push_back(http2::make_nv(kv.name, kv.value, kv.no_index));
  }

  auto method = http2::get_header(build_headers, ":method");
  assert(method);

  req->method = method->value;

  std::string trailer_names;
  if (!config.trailer.empty()) {
    trailer_names = config.trailer[0].name;
    for (size_t i = 1; i < config.trailer.size(); ++i) {
      trailer_names += ", ";
      trailer_names += config.trailer[i].name;
    }
    nva.push_back(http2::make_nv_ls("trailer", trailer_names));
  }

  int32_t stream_id;

  if (expect_continue) {
    stream_id = nghttp2_submit_headers(client->session, 0, -1, &req->pri_spec,
                                       nva.data(), nva.size(), req);
  } else {
    stream_id =
        nghttp2_submit_request(client->session, &req->pri_spec, nva.data(),
                               nva.size(), req->data_prd, req);
  }

  if (stream_id < 0) {
    std::cerr << "[ERROR] nghttp2_submit_"
              << (expect_continue ? "headers" : "request")
              << "() returned error: " << nghttp2_strerror(stream_id)
              << std::endl;
    return -1;
  }

  req->stream_id = stream_id;
  client->request_done(req);

  req->req_nva = std::move(build_headers);

  if (expect_continue) {
    auto timer = make_unique<ContinueTimer>(client->loop, req);
    req->continue_timer = std::move(timer);
  }

  return 0;
}
} // namespace

// Hung: commons
// ms
int hung_MAX_SEGMENTS = 2001;

// Hung: for clocks
int hung_sys_time = 0;
int hung_last_adapt_time[] = {0, 0};
int hung_client_seg = 0;

// Hung: temporary store => don't understand why
std::string hung_uri;
nghttp2_data_provider *hung_data_prd;
int64_t hung_data_length;
nghttp2_priority_spec hung_pri_spec;
Headers hung_headers;
nghttp2_priority_spec trung_pri_spec;

// Hung: measurement

double hung_inst_thrp = 1000;
int hung_cur_buff = 5000;
// int dang_cur_buff[]={5500,5500};

double thrp_est = 0, smoothedBW;

double hung_safety_margin = 0.1;
bool first_request = true;
bool first_adapt = true;

enum Thrp_est_method { Instant, Mix };
// Thrp_est_method      thrp_est_method;

// int                  hung_cur_rtt = 50;     // ik1 => 220ms
// int                  hung_K = 4;

// Hung: recorders
std::vector<int> hung_seg_recorder;
std::vector<int> hung_time_recorder;
std::vector<int> hung_rate_recorder;
std::vector<double> hung_thrp_recorder;
std::vector<int> hung_buff_recorder;
vector<double> time_download_recorder;

std::vector<double> thrp_est_recorder, smooth_thrp_recoder;
//-------------------------------------------------------
// double window_bits;
double safety_margin = 0.1;
// int num_of_segs =1;
// int request_corr_id[10000];
// int block_stream;
// bool set_block=true;
// int request1=0,request2=0;
nghttp2_data_provider *dang_data_prd;
int64_t dang_data_length;
Headers dang_headers;
std::string dang_uri;
nghttp2_priority_spec dang_pri_spec;
double curr_thrp = 0;
int dang_curr_request = 15;

const int num_of_tile = 6;
// bool need_sort=true;
// int need_request=0;
//-------------------------------------------------------
vector<int> dang_rate_set_orginal = {800,  1000, 2000, 3000,
                                     4000, 6000, 8000, 10000};
int NO_VER = dang_rate_set_orginal.size();
// vector<int> dang_rate_set_orginal = {877 ,1195 ,1771 ,2381 ,3152 ,4410 ,10085
// ,10583 ,17926};
// vector<int>
// dang_rate_set={20,30,60,80,110,150,220,300,390,550,1260,1320,2240};
// int weight[num_of_tile]={-1,-1,-1,-1,-1,-1,-1,-1};
// int choose_rate[num_of_tile];
// double square[num_of_tile]={0,0,0,0,12,20,38,30};
// double sorted_square[num_of_tile];
// int tile_index[num_of_tile];
vector<double> dang_thrp[num_of_tile];
vector<double> dang_bitrate[num_of_tile];

int dang_last_segment = 0;
// int dang_last_time=0;

// int dang_last_adapt_time=0;
double total_size_segment = 0;
int dang_first_response = 0;
int no_of_push_in_segment = 0;
int rate_continuous = 0;
int tile_in_segment = 6;
int count_tile = 0;
double dang_inst_thrp = 0;
double dang_temp_thrp = 0;
int start_measure_thrp;
int end_measure_thrp;
double total_data = 0;
int total_time = 0;
int hung_time_unit = 50;
ev_timer tev, dang_tev, delay_tev;
bool dang_connected = false;
bool dang_rst = false;
const int initial_segment = 1;
// int dang_tile_buffer=(initial_segment-1)*64;
// std::vector<int> dang_tile_buffer_record;
std::vector<double> dang_thrp_record2;
// const string
// first_tiles="20_21_28_29_36_37_44_45_3_4_5_6_11_12_13_14_19_22_27_30_35_38_43_46_51_52_53_54_59_60_61_62";
// const string first_tiles =
// "1_2_3_4_5_6_7_8_9_10_11_12_13_14_15_16_17_18_19_20_21_22_23_24";

// const string
// first_tiles="1_2_3_4_5_6_7_8_9_10_11_12_13_14_15_16_17_18_19_20_21_22_23_24_25_26_27_28_29_30_31_32_33_34_35_36_37_38_39_40_41_42_43_44_45_46_47_48_49_50_51_52_53_54_55_56_57_58_59_60_61_62_63_64";
string rate_string;

bool canceled = false;
int curr_segment = 0;
int seg_terminate = -1;
int no_request = 1;

int played = 1 - initial_segment;
bool dang_buffering = false;
ofstream play_file;
int time_start_play = 0;
int time_start_buffering;
ofstream buffering_file;
// ofstream segment_nonenough;
// ofstream buffer_detail;
// ofstream cnt_tile, data_summary;
ofstream GOP_buffer_file;
bool decide_cancel = false;
int data_playout = 0;
double total_rate_request;
double total_rate_downloaded;
vector<double> dang_segment_rate;
// int rate1 = 3152, rate2 = 2381 ;
int dang_face_rate[num_of_tile] = {3000, 2000, 2000, 2000, 2000, 800};
bool dang_buffering2 = false;

vector<int> region3;
std::vector<int> region1 = {0};
std::vector<int> region2 = {1, 2, 3, 4};
int region[num_of_tile + 1];
int dang_K = num_of_tile;
int last_K = dang_K;
// int total_time_rebuff = 0;
const int num_of_region = 3;
// bool finish_request [num_of_region + 1];
int last_region = 6;
// int num_of_tile1 = 0, num_of_tile2 = 0,num_of_tile3 = 0, unusable_tiles = 0;

// int num_region_request = 3;
int cnt_face = 0;
// vector<int> region [num_of_region + 1];
// int num_of_tile=64;
// bool terminate = false;
// int selected_rate[num_of_tile];
int curr_request_stream_id = 23;
int total_time_rebuff = 0;
int start_time_rebuff = 0;

const bool priority_enable = true;
const bool repri_enable = true;
const bool cancel_enable = true;
bool is_sent_cancel = false;
const int dang_sd = 5000;
const int NO_REQUEST = 80;
const int GOP_size = 5;
const int frame_per_s = 30;

double size_of_frame[num_of_tile];
int frame_buffer[num_of_tile] = {0, 0, 0, 0, 0, 0};
int face_stream_id[num_of_tile];
double data_frame_compenstation[num_of_tile] = {0, 0, 0, 0, 0, 0};

bool start_timer = false;
bool closed_connection = false;
int next_segment = 1;
bool terminate_session = false;
int limited_dl_time = dang_sd;
int last_limited_dl_time = dang_sd;
int dang_download_time;
bool repri_face[num_of_tile];
bool cancel_face[num_of_tile];
bool back_face_last_come = false;
bool is_face_0_finished = false;
int size_of_seg;
// int size_of_face [num_of_tile];
const int num_of_version = 8;
int dang_size_file[num_of_tile][num_of_version];
int dang_req_data_length[num_of_tile];
bool late_send_request = false;
bool finish_curr_seg = false;
bool restart_timer = false;
int time_start_send_request = 0;
int time_finish_request = 0;
int max_timer, cnt_timer = 0;
bool isFirstTime = true;
const int initial_buffer = 30;
// int stream_in_region [num_of_region];
//-------------------------------------------------------------------

// Hung: get rate and seg# from the uri
// int hung_get_rate_from_uri (std::string uri) {
//   return std::stoi(uri.substr(uri.rfind('_') + 1));
// }
// int hung_get_seg_from_uri (std::string uri) {
//   int l = uri.find('_') + 1;
//   int r = uri.find('_', l);
//   return std::stoi(uri.substr(l, r-l));
// }
int getIndexByRate(int rate) {
  for (int i = 0; i < dang_rate_set_orginal.size(); i++) {
    if (rate == dang_rate_set_orginal.at(i))
      return i;
  }
  return -1;
}

void set_region() {
  for (int i = 0; i < num_of_tile + 1; i++)
    // if(region[i] != 1 && region[i] != 2)
    region[i] = 3;
  for (int i = 0; i < region1.size(); i++)
    region[region1.at(i)] = 1;
  for (int i = 0; i < region2.size(); i++)
    region[region2.at(i)] = 2;

  for (int i = 1; i < num_of_tile + 1; i++)
    if (region[i] == 3)
      region3.push_back(i);
  // for(int i = 1 ; i<= num_of_tile ;i++)
  //  cout<<region[i]<<" ";
  // cout<<endl;
}
void init_setting() {
  // total_rate_request = (rate1 + 4 * rate2 + dang_rate_set_orginal.at(0) )/
  // num_of_tile;

  ifstream f;
  f.open("manifest.txt");
  for (int i = 0; i < num_of_tile; i++)
    for (int j = 0; j < num_of_version; j++) {
      f >> dang_size_file[i][j];
      // cout << dang_size_file[i][j] << " ";
    }
  // size_of_frame [0] = (1000.0 * rate1  ) / (num_of_tile * frame_per_s * 8.0);
  // size_of_frame [1] = (1000.0 * rate2  ) / (num_of_tile * frame_per_s * 8.0);
  // //size_of_frame [2] = (int)(1000.0 * dang_rate_set_orginal.at(0)) /
  // (num_of_tile * 30 * 8.0); size_of_frame [2] = (1000.0 *
  // dang_rate_set_orginal.at(0)  ) / (num_of_tile * frame_per_s * 8.0);
  size_of_seg = 0;
  for (int i = 0; i < num_of_tile; i++) {
    dang_req_data_length[i] = 0;
    int version = getIndexByRate(dang_face_rate[i]);
    size_of_seg += dang_size_file[i][version] * 8.0;
    size_of_frame[i] =
        dang_size_file[i][version] * 1000.0 / (dang_sd * frame_per_s);
    // cout << size_of_frame[i] << " ";
  }

  total_rate_request = size_of_seg * 1.0 / dang_sd;
  // size_of_seg *= dang_sd / 1000.0;
  // size_of_seg = total_rate_request * dang_sd;
  // for (int i = 0; i < num_of_tile; i++ )
  //  size_of_face [i] = 0;
}

void send_request_handle(struct ev_loop *loop, ev_timer *w, int revents);
void display_timer_handle(struct ev_loop *loop, ev_timer *w, int revents);
int dang_get_seg_from_uri(std::string uri) {
  int l = uri.find('_') + 1;
  int r = uri.find('_', l);
  return std::stoi(uri.substr(l, r - l));
}
int dang_get_face_from_uri(std::string uri) {
  return std::stoi(uri.substr(uri.rfind('_') + 1));
}
int dang_get_face_from_stream_id(int stream_id) {
  for (int i = 0; i < num_of_tile; i++) {
    if (face_stream_id[i] == stream_id)
      return i;
  }
}
int dang_get_rate_from_uri(std::string uri) {
  int l;
  string sub = uri;
  for (int i = 0; i < 3; i++) {
    l = sub.find('_') + 1;
    // cout<<l<<endl;
    // int r = uri.find('_', l);=
    sub = sub.substr(l);
    // cout<<sub<<endl;
  }
  int r = sub.find("_");
  // cout<<sub.find("_")<<endl;
  return std::stoi(sub.substr(0, r));
  // return std::stoi(uri.substr(l, r-l));
}
namespace {
void readcb(struct ev_loop *loop, ev_io *w, int revents) {
  auto client = static_cast<HttpClient *>(w->data);
  if (client->do_read() != 0) {
    client->disconnect();
  }
}
} // namespace

namespace {
void writecb(struct ev_loop *loop, ev_io *w, int revents) {
  // cout<<"dcmm sao eo ra"<<endl;
  auto client = static_cast<HttpClient *>(w->data);
  auto rv = client->do_write();
  if (rv == HttpClient::ERR_CONNECT_FAIL) {
    client->connect_fail();
    return;
  }
  if (rv != 0) {
    client->disconnect();
  }
}
} // namespace

namespace {
void timeoutcb(struct ev_loop *loop, ev_timer *w, int revents) {
  auto client = static_cast<HttpClient *>(w->data);
  std::cerr << "[ERROR] Timeout" << std::endl;
  client->disconnect();
}
} // namespace

namespace {
void settings_timeout_cb(struct ev_loop *loop, ev_timer *w, int revents) {
  auto client = static_cast<HttpClient *>(w->data);
  ev_timer_stop(loop, w);

  nghttp2_session_terminate_session(client->session, NGHTTP2_SETTINGS_TIMEOUT);

  client->signal_write();
}
} // namespace

HttpClient::HttpClient(const nghttp2_session_callbacks *callbacks,
                       struct ev_loop *loop, SSL_CTX *ssl_ctx)
    : wb(&mcpool),
      session(nullptr),
      callbacks(callbacks),
      loop(loop),
      ssl_ctx(ssl_ctx),
      ssl(nullptr),
      addrs(nullptr),
      next_addr(nullptr),
      cur_addr(nullptr),
      complete(0),
      success(0),
      settings_payloadlen(0),
      state(ClientState::IDLE),
      upgrade_response_status_code(0),
      fd(-1),
      upgrade_response_complete(false) {
  ev_io_init(&wev, writecb, 0, EV_WRITE);
  ev_io_init(&rev, readcb, 0, EV_READ);

  ev_timer_init(&tev, send_request_handle, 0, 1.0 / 1000);
  ev_timer_start(loop, &tev);

  // play_file.open("/home/haidang/client/Output/play_file.txt");
  // buffering_file.open("/home/haidang/client/Output/buffering.txt");
  // //segment_nonenough.open("/home/haidang/client/Output/segment_nonenough.txt");
  // //data_summary.open("/home/haidang/client/Output/data_summary.txt");
  // //buffer_detail.open("/home/haidang/client/Output/buffer_detail.txt");
  // buffering_file << "Segment\tTile\tTime Buffering" << endl;
  // segment_nonenough << "Segment\tNum downloaded tile\tTile in region 1\tTile
  // in region 2"<<endl; buffer_detail<<"Time
  // playout\tRegion1\tRegion2\tRegion3\tUnsuable Tiles"<<endl;
  // cnt_tile.open("/home/haidang/client/Output/cnt_tile.txt");
  // cnt_tile<<"Segment\tNum of tile"<<endl;
  // data_summary<<"Segment\tData downloaded\tData playout"<<endl;
  GOP_buffer_file.open("/home/typhoon/Desktop/StreamPriority/test_priority/"
                       "client/Output/GOP_buffer_file.txt");
  GOP_buffer_file << "Time\t";
  for (int i = 0; i < num_of_tile; i++)
    GOP_buffer_file << "Face " << i << '\t';
  GOP_buffer_file << endl;
  set_region();

  // cout<<"HttpClient"<<endl;
  wev.data = this;
  rev.data = this;

  ev_timer_init(&wt, timeoutcb, 0., config.timeout);
  ev_timer_init(&rt, timeoutcb, 0., config.timeout);

  wt.data = this;
  rt.data = this;

  ev_timer_init(&settings_timer, settings_timeout_cb, 0., 10.);

  settings_timer.data = this;
}

HttpClient::~HttpClient() {
  disconnect();

  if (addrs) {
    freeaddrinfo(addrs);
    addrs = nullptr;
    next_addr = nullptr;
  }
}

bool HttpClient::need_upgrade() const {
  return config.upgrade && scheme == "http";
}

int HttpClient::resolve_host(const std::string &host, uint16_t port) {
  int rv;
  this->host = host;
  addrinfo hints{};
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_protocol = 0;
  hints.ai_flags = AI_ADDRCONFIG;
  rv = getaddrinfo(host.c_str(), util::utos(port).c_str(), &hints, &addrs);
  if (rv != 0) {
    std::cerr << "[ERROR] getaddrinfo() failed: " << gai_strerror(rv)
              << std::endl;
    return -1;
  }
  if (addrs == nullptr) {
    std::cerr << "[ERROR] No address returned" << std::endl;
    return -1;
  }
  next_addr = addrs;
  return 0;
}

int HttpClient::initiate_connection() {
  int rv;

  cur_addr = nullptr;
  while (next_addr) {
    cur_addr = next_addr;
    next_addr = next_addr->ai_next;
    fd = util::create_nonblock_socket(cur_addr->ai_family);
    if (fd == -1) {
      continue;
    }

    if (ssl_ctx) {
      // We are establishing TLS connection.
      ssl = SSL_new(ssl_ctx);
      if (!ssl) {
        std::cerr << "[ERROR] SSL_new() failed: "
                  << ERR_error_string(ERR_get_error(), nullptr) << std::endl;
        return -1;
      }

      SSL_set_fd(ssl, fd);
      SSL_set_connect_state(ssl);

      // If the user overrode the :authority or host header, use that
      // value for the SNI extension
      const char *host_string = nullptr;
      auto i =
          std::find_if(std::begin(config.headers), std::end(config.headers),
                       [](const Header &nv) {
                         return ":authority" == nv.name || "host" == nv.name;
                       });
      if (i != std::end(config.headers)) {
        host_string = (*i).value.c_str();
      } else {
        host_string = host.c_str();
      }

      if (!util::numeric_host(host_string)) {
        SSL_set_tlsext_host_name(ssl, host_string);
      }
    }

    rv = connect(fd, cur_addr->ai_addr, cur_addr->ai_addrlen);

    if (rv != 0 && errno != EINPROGRESS) {
      if (ssl) {
        SSL_free(ssl);
        ssl = nullptr;
      }
      close(fd);
      fd = -1;
      continue;
    }
    break;
  }

  if (fd == -1) {
    return -1;
  }

  writefn = &HttpClient::connected;

  if (need_upgrade()) {
    on_readfn = &HttpClient::on_upgrade_read;
    on_writefn = &HttpClient::on_upgrade_connect;
  } else {
    on_readfn = &HttpClient::on_read;
    on_writefn = &HttpClient::on_write;
  }
  // ev_timer_init(&tev, hung_tick_cb,0.,10.);
  // tev.repeat = hung_time_unit / 1000.0;
  // if(dang_connected)
  // ev_timer_again(loop,&tev);
  ev_io_set(&rev, fd, EV_READ);
  ev_io_set(&wev, fd, EV_WRITE);

  ev_io_start(loop, &wev);

  ev_timer_again(loop, &wt);

  return 0;
}

void HttpClient::disconnect() {
  cout << "Close connection" << endl;
  state = ClientState::IDLE;
  // play_file.close();
  for (auto req = std::begin(reqvec); req != std::end(reqvec); ++req) {
    if ((*req)->continue_timer) {
      (*req)->continue_timer->stop();
    }
  }
  closed_connection = true;

  ev_timer_stop(loop, &settings_timer);

  ev_timer_stop(loop, &rt);
  ev_timer_stop(loop, &wt);
  ev_timer_stop(loop, &tev);
  // ev_timer_stop(loop,&dang_tev);
  ev_io_stop(loop, &rev);
  ev_io_stop(loop, &wev);

  nghttp2_session_del(session);
  session = nullptr;

  if (ssl) {
    SSL_set_shutdown(ssl, SSL_RECEIVED_SHUTDOWN);
    ERR_clear_error();
    SSL_shutdown(ssl);
    SSL_free(ssl);
    ssl = nullptr;
  }

  if (fd != -1) {
    shutdown(fd, SHUT_WR);
    close(fd);
    fd = -1;
  }
}

int HttpClient::read_clear() {
  ev_timer_again(loop, &rt);

  std::array<uint8_t, 8_k> buf;

  for (;;) {
    ssize_t nread;
    while ((nread = read(fd, buf.data(), buf.size())) == -1 && errno == EINTR)
      ;
    if (nread == -1) {
      if (errno == EAGAIN || errno == EWOULDBLOCK) {
        return 0;
      }
      return -1;
    }

    if (nread == 0) {
      return -1;
    }

    if (on_readfn(*this, buf.data(), nread) != 0) {
      return -1;
    }
  }

  return 0;
}

int HttpClient::write_clear() {
  ev_timer_again(loop, &rt);

  std::array<struct iovec, 2> iov;

  for (;;) {
    if (on_writefn(*this) != 0) {
      return -1;
    }

    auto iovcnt = wb.riovec(iov.data(), iov.size());

    if (iovcnt == 0) {
      break;
    }

    ssize_t nwrite;
    while ((nwrite = writev(fd, iov.data(), iovcnt)) == -1 && errno == EINTR)
      ;
    if (nwrite == -1) {
      if (errno == EAGAIN || errno == EWOULDBLOCK) {
        ev_io_start(loop, &wev);
        ev_timer_again(loop, &wt);
        return 0;
      }
      return -1;
    }

    wb.drain(nwrite);
  }

  ev_io_stop(loop, &wev);
  ev_timer_stop(loop, &wt);

  return 0;
}

int HttpClient::noop() { return 0; }

void HttpClient::connect_fail() {
  if (state == ClientState::IDLE) {
    std::cerr << "[ERROR] Could not connect to the address "
              << util::numeric_name(cur_addr->ai_addr, cur_addr->ai_addrlen)
              << std::endl;
  }
  auto cur_state = state;
  disconnect();
  if (cur_state == ClientState::IDLE) {
    if (initiate_connection() == 0) {
      std::cerr << "Trying next address "
                << util::numeric_name(cur_addr->ai_addr, cur_addr->ai_addrlen)
                << std::endl;
    }
  }
}

int HttpClient::connected() {
  if (!util::check_socket_connected(fd)) {
    return ERR_CONNECT_FAIL;
  }
  init_setting();
  if (config.verbose) {
    print_timer();
    std::cout << " Connected" << std::endl;
    dang_connected = true;
  }

  state = ClientState::CONNECTED;

  ev_io_start(loop, &rev);
  ev_io_stop(loop, &wev);
  ev_timer_again(loop, &tev);
  ev_timer_again(loop, &rt);
  ev_timer_stop(loop, &wt);

  if (ssl) {
    readfn = &HttpClient::tls_handshake;
    writefn = &HttpClient::tls_handshake;

    return do_write();
  }

  readfn = &HttpClient::read_clear;
  writefn = &HttpClient::write_clear;

  if (need_upgrade()) {
    htp = make_unique<http_parser>();
    http_parser_init(htp.get(), HTTP_RESPONSE);
    htp->data = this;

    return do_write();
  }

  if (connection_made() != 0) {
    return -1;
  }

  return 0;
}

namespace {
size_t populate_settings(nghttp2_settings_entry *iv) {
  size_t niv = 2;

  iv[0].settings_id = NGHTTP2_SETTINGS_MAX_CONCURRENT_STREAMS;
  iv[0].value = config.max_concurrent_streams;

  iv[1].settings_id = NGHTTP2_SETTINGS_INITIAL_WINDOW_SIZE;
  if (config.window_bits != -1) {
    iv[1].value = (1 << config.window_bits) - 1;
  } else {
    iv[1].value = NGHTTP2_INITIAL_WINDOW_SIZE;
  }

  if (config.header_table_size >= 0) {
    if (config.min_header_table_size < config.header_table_size) {
      iv[niv].settings_id = NGHTTP2_SETTINGS_HEADER_TABLE_SIZE;
      iv[niv].value = config.min_header_table_size;
      ++niv;
    }

    iv[niv].settings_id = NGHTTP2_SETTINGS_HEADER_TABLE_SIZE;
    iv[niv].value = config.header_table_size;
    ++niv;
  }

  if (config.no_push) {
    iv[niv].settings_id = NGHTTP2_SETTINGS_ENABLE_PUSH;
    iv[niv].value = 0;
    ++niv;
  }
  iv[niv].settings_id = NGHTTP2_SETTINGS_MAX_FRAME_SIZE;
  iv[niv].value = 1 << 20;
  ++niv;
  return niv;
}
} // namespace

int HttpClient::on_upgrade_connect() {
  ssize_t rv;
  record_connect_end_time();
  assert(!reqvec.empty());
  std::array<nghttp2_settings_entry, 16> iv;
  size_t niv = populate_settings(iv.data());
  assert(settings_payload.size() >= 8 * niv);
  rv = nghttp2_pack_settings_payload(settings_payload.data(),
                                     settings_payload.size(), iv.data(), niv);
  if (rv < 0) {
    return -1;
  }
  settings_payloadlen = rv;
  auto token68 =
      base64::encode(std::begin(settings_payload),
                     std::begin(settings_payload) + settings_payloadlen);
  util::to_token68(token68);

  std::string req;
  if (reqvec[0]->data_prd) {
    // If the request contains upload data, use OPTIONS * to upgrade
    req = "OPTIONS *";
  } else {
    auto meth = std::find_if(
        std::begin(config.headers), std::end(config.headers),
        [](const Header &kv) { return util::streq_l(":method", kv.name); });

    if (meth == std::end(config.headers)) {
      req = "GET ";
      reqvec[0]->method = "GET";
    } else {
      req = (*meth).value;
      req += ' ';
      reqvec[0]->method = (*meth).value;
    }
    req += reqvec[0]->make_reqpath();
  }

  auto headers = Headers{{"host", hostport},
                         {"connection", "Upgrade, HTTP2-Settings"},
                         {"upgrade", NGHTTP2_CLEARTEXT_PROTO_VERSION_ID},
                         {"http2-settings", token68},
                         {"accept", "*/*"},
                         {"user-agent", "nghttp2/" NGHTTP2_VERSION}};
  auto initial_headerslen = headers.size();

  for (auto &kv : config.headers) {
    size_t i;
    if (kv.name.empty() || kv.name[0] == ':') {
      continue;
    }
    for (i = 0; i < initial_headerslen; ++i) {
      if (kv.name == headers[i].name) {
        headers[i].value = kv.value;
        break;
      }
    }
    if (i < initial_headerslen) {
      continue;
    }
    headers.emplace_back(kv.name, kv.value, kv.no_index);
  }

  req += " HTTP/1.1\r\n";

  for (auto &kv : headers) {
    req += kv.name;
    req += ": ";
    req += kv.value;
    req += "\r\n";
  }
  req += "\r\n";

  wb.append(req);

  if (config.verbose) {
    print_timer();
    std::cout << " HTTP Upgrade request\n" << req << std::endl;
  }

  if (!reqvec[0]->data_prd) {
    // record request time if this is a part of real request.
    reqvec[0]->record_request_start_time();
    reqvec[0]->req_nva = std::move(headers);
  }

  on_writefn = &HttpClient::noop;

  signal_write();

  return 0;
}

int HttpClient::on_upgrade_read(const uint8_t *data, size_t len) {
  int rv;

  auto nread = http_parser_execute(htp.get(), &htp_hooks,
                                   reinterpret_cast<const char *>(data), len);

  if (config.verbose) {
    std::cout.write(reinterpret_cast<const char *>(data), nread);
  }

  auto htperr = HTTP_PARSER_ERRNO(htp.get());

  if (htperr != HPE_OK) {
    std::cerr << "[ERROR] Failed to parse HTTP Upgrade response header: "
              << "(" << http_errno_name(htperr) << ") "
              << http_errno_description(htperr) << std::endl;
    return -1;
  }

  if (!upgrade_response_complete) {
    return 0;
  }

  if (config.verbose) {
    std::cout << std::endl;
  }

  if (upgrade_response_status_code != 101) {
    std::cerr << "[ERROR] HTTP Upgrade failed" << std::endl;

    return -1;
  }

  if (config.verbose) {
    print_timer();
    std::cout << " HTTP Upgrade success" << std::endl;
  }

  on_readfn = &HttpClient::on_read;
  on_writefn = &HttpClient::on_write;

  rv = connection_made();
  if (rv != 0) {
    return rv;
  }

  // Read remaining data in the buffer because it is not notified
  // callback anymore.
  rv = on_readfn(*this, data + nread, len - nread);
  if (rv != 0) {
    return rv;
  }

  return 0;
}

int HttpClient::do_read() { return readfn(*this); }
int HttpClient::do_write() { return writefn(*this); }

int HttpClient::connection_made() {
  int rv;

  if (!need_upgrade()) {
    record_connect_end_time();
  }

  if (ssl) {
    // Check NPN or ALPN result
    const unsigned char *next_proto = nullptr;
    unsigned int next_proto_len;
    SSL_get0_next_proto_negotiated(ssl, &next_proto, &next_proto_len);
    for (int i = 0; i < 2; ++i) {
      if (next_proto) {
        auto proto = StringRef{next_proto, next_proto_len};
        if (config.verbose) {
          std::cout << "The negotiated protocol: " << proto << std::endl;
        }
        if (!util::check_h2_is_selected(proto)) {
          next_proto = nullptr;
        }
        break;
      }
#if OPENSSL_VERSION_NUMBER >= 0x10002000L
      SSL_get0_alpn_selected(ssl, &next_proto, &next_proto_len);
#else  // OPENSSL_VERSION_NUMBER < 0x10002000L
      break;
#endif // OPENSSL_VERSION_NUMBER < 0x10002000L
    }
    if (!next_proto) {
      print_protocol_nego_error();
      return -1;
    }
  }

  rv = nghttp2_session_client_new2(&session, callbacks, this,
                                   config.http2_option);

  if (rv != 0) {
    return -1;
  }
  if (need_upgrade()) {
    // Adjust stream user-data depending on the existence of upload
    // data
    Request *stream_user_data = nullptr;
    if (!reqvec[0]->data_prd) {
      stream_user_data = reqvec[0].get();
    }
    // If HEAD is used, that is only when user specified it with -H
    // option.
    auto head_request = stream_user_data && stream_user_data->method == "HEAD";
    rv = nghttp2_session_upgrade2(session, settings_payload.data(),
                                  settings_payloadlen, head_request,
                                  stream_user_data);
    if (rv != 0) {
      std::cerr << "[ERROR] nghttp2_session_upgrade() returned error: "
                << nghttp2_strerror(rv) << std::endl;
      return -1;
    }
    if (stream_user_data) {
      stream_user_data->stream_id = 1;
      request_done(stream_user_data);
    }
  }
  // If upgrade succeeds, the SETTINGS value sent with
  // HTTP2-Settings header field has already been submitted to
  // session object.
  if (!need_upgrade()) {
    std::array<nghttp2_settings_entry, 16> iv;
    auto niv = populate_settings(iv.data());
    rv = nghttp2_submit_settings(session, NGHTTP2_FLAG_NONE, iv.data(), niv);
    if (rv != 0) {
      return -1;
    }
  }
  if (!config.no_dep) {
    // Create anchor stream nodes
    nghttp2_priority_spec pri_spec;

    for (auto &anchor : anchors) {
      nghttp2_priority_spec_init(&pri_spec, anchor.dep_stream_id, anchor.weight,
                                 0);
      rv = nghttp2_submit_priority(session, NGHTTP2_FLAG_NONE, anchor.stream_id,
                                   &pri_spec);
      if (rv != 0) {
        return -1;
      }
    }

    rv = nghttp2_session_set_next_stream_id(
        session, anchors[ANCHOR_FOLLOWERS].stream_id + 2);
    if (rv != 0) {
      return -1;
    }

    if (need_upgrade() && !reqvec[0]->data_prd) {
      // Amend the priority because we cannot send priority in
      // HTTP/1.1 Upgrade.
      auto &anchor = anchors[ANCHOR_FOLLOWERS];
      nghttp2_priority_spec_init(&pri_spec, anchor.stream_id,
                                 reqvec[0]->pri_spec.weight, 0);

      rv = nghttp2_submit_priority(session, NGHTTP2_FLAG_NONE, 1, &pri_spec);
      if (rv != 0) {
        return -1;
      }
    }
  } else if (need_upgrade() && !reqvec[0]->data_prd &&
             reqvec[0]->pri_spec.weight != NGHTTP2_DEFAULT_WEIGHT) {
    // Amend the priority because we cannot send priority in HTTP/1.1
    // Upgrade.
    nghttp2_priority_spec pri_spec;

    nghttp2_priority_spec_init(&pri_spec, 0, reqvec[0]->pri_spec.weight, 0);

    rv = nghttp2_submit_priority(session, NGHTTP2_FLAG_NONE, 1, &pri_spec);
    if (rv != 0) {
      return -1;
    }
  }

  ev_timer_again(loop, &settings_timer);

  if (config.connection_window_bits != -1) {
    int32_t window_size = (1 << config.connection_window_bits) - 1;
    rv = nghttp2_session_set_local_window_size(session, NGHTTP2_FLAG_NONE, 0,
                                               window_size);
    if (rv != 0) {
      return -1;
    }
  }
  // Adjust first request depending on the existence of the upload
  // data
  for (auto i = std::begin(reqvec) + (need_upgrade() && !reqvec[0]->data_prd);
       i != std::end(reqvec); ++i) {
    if (submit_request(this, config.headers, (*i).get()) != 0) {
      return -1;
    }
    // Hung:
    dang_headers = config.headers;
  }

  signal_write();

  return 0;
}

int HttpClient::on_read(const uint8_t *data, size_t len) {
  if (config.hexdump) {
    util::hexdump(stdout, data, len);
  }

  auto rv = nghttp2_session_mem_recv(session, data, len);
  if (rv < 0) {
    std::cerr << "[ERROR] nghttp2_session_mem_recv() returned error: "
              << nghttp2_strerror(rv) << std::endl;
    return -1;
  }

  assert(static_cast<size_t>(rv) == len);

  if (nghttp2_session_want_read(session) == 0 &&
      nghttp2_session_want_write(session) == 0 && wb.rleft() == 0) {
    return -1;
  }

  signal_write();

  return 0;
}

int HttpClient::on_write() {
  for (;;) {
    if (wb.rleft() >= 16384) {
      return 0;
    }

    const uint8_t *data;
    auto len = nghttp2_session_mem_send(session, &data);
    if (len < 0) {
      std::cerr << "[ERROR] nghttp2_session_send() returned error: "
                << nghttp2_strerror(len) << std::endl;
      return -1;
    }

    if (len == 0) {
      break;
    }

    wb.append(data, len);
  }

  if (nghttp2_session_want_read(session) == 0 &&
      nghttp2_session_want_write(session) == 0 && wb.rleft() == 0) {
    return -1;
  }

  return 0;
}

int HttpClient::tls_handshake() {
  ev_timer_again(loop, &rt);

  ERR_clear_error();

  auto rv = SSL_do_handshake(ssl);

  if (rv <= 0) {
    auto err = SSL_get_error(ssl, rv);
    switch (err) {
    case SSL_ERROR_WANT_READ:
      ev_io_stop(loop, &wev);
      ev_timer_stop(loop, &wt);
      return 0;
    case SSL_ERROR_WANT_WRITE:
      ev_io_start(loop, &wev);
      ev_timer_again(loop, &wt);
      return 0;
    default:
      return -1;
    }
  }

  ev_io_stop(loop, &wev);
  ev_timer_stop(loop, &wt);

  readfn = &HttpClient::read_tls;
  writefn = &HttpClient::write_tls;

  if (connection_made() != 0) {
    return -1;
  }

  return 0;
}

int HttpClient::read_tls() {
  ev_timer_again(loop, &rt);

  ERR_clear_error();

  std::array<uint8_t, 8_k> buf;
  for (;;) {
    auto rv = SSL_read(ssl, buf.data(), buf.size());

    if (rv <= 0) {
      auto err = SSL_get_error(ssl, rv);
      switch (err) {
      case SSL_ERROR_WANT_READ:
        return 0;
      case SSL_ERROR_WANT_WRITE:
        // renegotiation started
        return -1;
      default:
        return -1;
      }
    }

    if (on_readfn(*this, buf.data(), rv) != 0) {
      return -1;
    }
  }
}

int HttpClient::write_tls() {
  ev_timer_again(loop, &rt);

  ERR_clear_error();

  struct iovec iov;

  for (;;) {
    if (on_writefn(*this) != 0) {
      return -1;
    }

    auto iovcnt = wb.riovec(&iov, 1);

    if (iovcnt == 0) {
      break;
    }

    auto rv = SSL_write(ssl, iov.iov_base, iov.iov_len);

    if (rv <= 0) {
      auto err = SSL_get_error(ssl, rv);
      switch (err) {
      case SSL_ERROR_WANT_READ:
        // renegotiation started
        return -1;
      case SSL_ERROR_WANT_WRITE:
        ev_io_start(loop, &wev);
        ev_timer_again(loop, &wt);
        return 0;
      default:
        return -1;
      }
    }

    wb.drain(rv);
  }

  ev_io_stop(loop, &wev);
  ev_timer_stop(loop, &wt);

  return 0;
}

void HttpClient::signal_write() { ev_io_start(loop, &wev); }

bool HttpClient::all_requests_processed() const {
  return complete == reqvec.size();
}

void HttpClient::update_hostport() {
  if (reqvec.empty()) {
    return;
  }
  scheme = util::get_uri_field(reqvec[0]->uri.c_str(), reqvec[0]->u, UF_SCHEMA)
               .str();
  std::stringstream ss;
  if (reqvec[0]->is_ipv6_literal_addr()) {
    // we may have zone ID, which must start with "%25", or "%".  RFC
    // 6874 defines "%25" only, and just "%" is allowed for just
    // convenience to end-user input.
    auto host =
        util::get_uri_field(reqvec[0]->uri.c_str(), reqvec[0]->u, UF_HOST);
    auto end = std::find(std::begin(host), std::end(host), '%');
    ss << "[";
    ss.write(host.c_str(), end - std::begin(host));
    ss << "]";
  } else {
    util::write_uri_field(ss, reqvec[0]->uri.c_str(), reqvec[0]->u, UF_HOST);
  }
  if (util::has_uri_field(reqvec[0]->u, UF_PORT) &&
      reqvec[0]->u.port !=
          util::get_default_port(reqvec[0]->uri.c_str(), reqvec[0]->u)) {
    ss << ":" << reqvec[0]->u.port;
  }
  hostport = ss.str();
}

bool HttpClient::add_request(const std::string &uri,
                             const nghttp2_data_provider *data_prd,
                             int64_t data_length,
                             const nghttp2_priority_spec &pri_spec, int level) {
  http_parser_url u{};
  if (http_parser_parse_url(uri.c_str(), uri.size(), 0, &u) != 0) {
    return false;
  }

  // Hung_comment: Tam thoi bo cai check duplication nay di (no cache)
  //  if (path_cache.count(uri)) {
  //    return false;
  //  }

  if (config.multiply == 1) {
    path_cache.insert(uri);
  }

  reqvec.push_back(
      make_unique<Request>(uri, u, data_prd, data_length, pri_spec, level));
  return true;
}

void HttpClient::record_start_time() {
  timing.system_start_time = std::chrono::system_clock::now();
  timing.start_time = get_time();
}

void HttpClient::record_domain_lookup_end_time() {
  timing.domain_lookup_end_time = get_time();
}

void HttpClient::record_connect_end_time() {
  timing.connect_end_time = get_time();
}

void HttpClient::request_done(Request *req) {
  if (req->stream_id % 2 == 0) {
    return;
  }
}

#ifdef HAVE_JANSSON
void HttpClient::output_har(FILE *outfile) {
  static auto PAGE_ID = "page_0";

  auto root = json_object();
  auto log = json_object();
  json_object_set_new(root, "log", log);
  json_object_set_new(log, "version", json_string("1.2"));

  auto creator = json_object();
  json_object_set_new(log, "creator", creator);

  json_object_set_new(creator, "name", json_string("nghttp"));
  json_object_set_new(creator, "version", json_string(NGHTTP2_VERSION));

  auto pages = json_array();
  json_object_set_new(log, "pages", pages);

  auto page = json_object();
  json_array_append_new(pages, page);

  json_object_set_new(
      page, "startedDateTime",
      json_string(util::format_iso8601(timing.system_start_time).c_str()));
  json_object_set_new(page, "id", json_string(PAGE_ID));
  json_object_set_new(page, "title", json_string(""));

  json_object_set_new(page, "pageTimings", json_object());

  auto entries = json_array();
  json_object_set_new(log, "entries", entries);

  auto dns_delta = std::chrono::duration_cast<std::chrono::microseconds>(
                       timing.domain_lookup_end_time - timing.start_time)
                       .count() /
                   1000.0;
  auto connect_delta =
      std::chrono::duration_cast<std::chrono::microseconds>(
          timing.connect_end_time - timing.domain_lookup_end_time)
          .count() /
      1000.0;

  for (size_t i = 0; i < reqvec.size(); ++i) {
    auto &req = reqvec[i];

    if (req->timing.state != RequestState::ON_COMPLETE) {
      continue;
    }

    auto entry = json_object();
    json_array_append_new(entries, entry);

    auto &req_timing = req->timing;
    auto request_time =
        (i == 0) ? timing.system_start_time
                 : timing.system_start_time +
                       std::chrono::duration_cast<
                           std::chrono::system_clock::duration>(
                           req_timing.request_start_time - timing.start_time);

    auto wait_delta =
        std::chrono::duration_cast<std::chrono::microseconds>(
            req_timing.response_start_time - req_timing.request_start_time)
            .count() /
        1000.0;
    auto receive_delta =
        std::chrono::duration_cast<std::chrono::microseconds>(
            req_timing.response_end_time - req_timing.response_start_time)
            .count() /
        1000.0;

    auto time_sum =
        std::chrono::duration_cast<std::chrono::microseconds>(
            (i == 0) ? (req_timing.response_end_time - timing.start_time)
                     : (req_timing.response_end_time -
                        req_timing.request_start_time))
            .count() /
        1000.0;

    json_object_set_new(
        entry, "startedDateTime",
        json_string(util::format_iso8601(request_time).c_str()));
    json_object_set_new(entry, "time", json_real(time_sum));

    auto pushed = req->stream_id % 2 == 0;

    json_object_set_new(entry, "comment",
                        json_string(pushed ? "Pushed Object" : ""));

    auto request = json_object();
    json_object_set_new(entry, "request", request);

    auto req_headers = json_array();
    json_object_set_new(request, "headers", req_headers);

    for (auto &nv : req->req_nva) {
      auto hd = json_object();
      json_array_append_new(req_headers, hd);

      json_object_set_new(hd, "name", json_string(nv.name.c_str()));
      json_object_set_new(hd, "value", json_string(nv.value.c_str()));
    }

    json_object_set_new(request, "method", json_string(req->method.c_str()));
    json_object_set_new(request, "url", json_string(req->uri.c_str()));
    json_object_set_new(request, "httpVersion", json_string("HTTP/2.0"));
    json_object_set_new(request, "cookies", json_array());
    json_object_set_new(request, "queryString", json_array());
    json_object_set_new(request, "headersSize", json_integer(-1));
    json_object_set_new(request, "bodySize", json_integer(-1));

    auto response = json_object();
    json_object_set_new(entry, "response", response);

    auto res_headers = json_array();
    json_object_set_new(response, "headers", res_headers);

    for (auto &nv : req->res_nva) {
      auto hd = json_object();
      json_array_append_new(res_headers, hd);

      json_object_set_new(hd, "name", json_string(nv.name.c_str()));
      json_object_set_new(hd, "value", json_string(nv.value.c_str()));
    }

    json_object_set_new(response, "status", json_integer(req->status));
    json_object_set_new(response, "statusText", json_string(""));
    json_object_set_new(response, "httpVersion", json_string("HTTP/2.0"));
    json_object_set_new(response, "cookies", json_array());

    auto content = json_object();
    json_object_set_new(response, "content", content);

    json_object_set_new(content, "size", json_integer(req->response_len));

    auto content_type_ptr = http2::get_header(req->res_nva, "content-type");

    const char *content_type = "";
    if (content_type_ptr) {
      content_type = content_type_ptr->value.c_str();
    }

    json_object_set_new(content, "mimeType", json_string(content_type));

    json_object_set_new(response, "redirectURL", json_string(""));
    json_object_set_new(response, "headersSize", json_integer(-1));
    json_object_set_new(response, "bodySize", json_integer(-1));
    json_object_set_new(entry, "cache", json_object());

    auto timings = json_object();
    json_object_set_new(entry, "timings", timings);

    auto dns_timing = (i == 0) ? dns_delta : 0;
    auto connect_timing = (i == 0) ? connect_delta : 0;

    json_object_set_new(timings, "dns", json_real(dns_timing));
    json_object_set_new(timings, "connect", json_real(connect_timing));

    json_object_set_new(timings, "blocked", json_real(0.0));
    json_object_set_new(timings, "send", json_real(0.0));
    json_object_set_new(timings, "wait", json_real(wait_delta));
    json_object_set_new(timings, "receive", json_real(receive_delta));

    json_object_set_new(entry, "pageref", json_string(PAGE_ID));
    json_object_set_new(entry, "connection",
                        json_string(util::utos(req->stream_id).c_str()));
  }

  json_dumpf(root, outfile, JSON_PRESERVE_ORDER | JSON_INDENT(2));
  json_decref(root);
}
#endif // HAVE_JANSSON

namespace {
void update_html_parser(HttpClient *client, Request *req, const uint8_t *data,
                        size_t len, int fin) {
  if (!req->html_parser) {
    return;
  }
  req->update_html_parser(data, len, fin);

  for (auto &p : req->html_parser->get_links()) {
    auto uri = strip_fragment(p.first.c_str());
    auto res_type = p.second;

    http_parser_url u{};
    if (http_parser_parse_url(uri.c_str(), uri.size(), 0, &u) == 0 &&
        util::fieldeq(uri.c_str(), u, req->uri.c_str(), req->u, UF_SCHEMA) &&
        util::fieldeq(uri.c_str(), u, req->uri.c_str(), req->u, UF_HOST) &&
        util::porteq(uri.c_str(), u, req->uri.c_str(), req->u)) {
      // No POST data for assets
      auto pri_spec = resolve_dep(res_type);

      if (client->add_request(uri, nullptr, 0, pri_spec, req->level + 1)) {

        submit_request(client, config.headers, client->reqvec.back().get());
      }
    }
  }
  req->html_parser->clear_links();
}
} // namespace

namespace {
HttpClient *get_client(void *user_data) {
  return static_cast<HttpClient *>(user_data);
}
} // namespace

void trung_adaptation(HttpClient *client, nghttp2_session *session, int iter_id,
                      double iter_down_elapsed_time);
namespace {
int on_data_chunk_recv_callback(nghttp2_session *session, uint8_t flags,
                                int32_t stream_id, const uint8_t *data,
                                size_t len, void *user_data) {
  auto client = get_client(user_data);
  auto req = static_cast<Request *>(
      nghttp2_session_get_stream_user_data(session, stream_id));

  if (!req) {
    return 0;
  }

  if (config.verbose >= 2) {
    verbose_on_data_chunk_recv_callback(session, flags, stream_id, data, len,
                                        user_data);
  }

  req->response_len += len;

  //////////////////////////////////////////////////
  int sid, fid, layerId, i, j;
  double iter_down_elapsed_time;
  bool STREAM_FIN_FLAG;
  // /* update per tile downloaded data */
  //   if(metric.RequestStreamInfo.find(stream_id) !=
  //   metric.RequestStreamInfo.end()){

  //     /* Determine segment/face ids */
  //     sid = metric.RequestStreamInfo.find(stream_id)->second->sid;
  //     fid = metric.RequestStreamInfo.find(stream_id)->second->fid;
  //     layerId = metric.RequestStreamInfo.find(stream_id)->second->lid;

  //     /* updating downloaded data amount of each tile in kbits */
  //     metric.face_dow_data[sid][fid][layerId] += len * 8.0/1000;
  //     cout << "data[" << sid << "][" << fid << "][" << layerId << "]=" << len
  //     * 8.0/1000 << endl;

  //     /* measure elapsed time since the current iteration starts */
  //     iter_down_elapsed_time =
  //     std::chrono::duration_cast<std::chrono::milliseconds>(
  //         get_time() - client->timing.connect_end_time).count() -
  //         metric.iter_start_time[iter_id] ;

  //     /* measure time elapsed since 'connect_end_time */
  //     auto t_now = std::chrono::duration_cast<std::chrono::microseconds>(
  //         get_time() - client->timing.connect_end_time).count()/1000.0;
  //     cout << t_now << ", " << player.seg_playback_deadline[sid] << endl;

  //     // if(meta.ALG == 2){
  //     if(meta.ALG == 2 || meta.ALG == 3 || meta.ALG == 4 || meta.ALG == 5){
  //       /* terminate late tile */
  //   if (sid > 0 && fid > 0 && (t_now >= player.seg_playback_deadline[sid])){
  //       cout << metric.face_stream_ID[sid][fid][layerId] << "," <<
  //       metric.face_push_stream_id[sid][fid][layerId] << endl;
  //         cout << "========== Cancelling stream " << stream_id << endl;
  //         cancelled_request_stream.push_back(stream_id);
  //         auto ret = nghttp2_submit_rst_stream(session, NGHTTP2_FLAG_NONE,
  //         stream_id, NGHTTP2_CANCEL); cout << "ret=" << ret << endl;

  //       // Find and canncel all active streams //
  //     //   for(i=0; i < metric.iter_req_stream[iter_id].size(); i++){
  //     //     STREAM_FIN_FLAG = false;
  //     //     for(j=0; j < metric.iter_req_stream_fin[iter_id].size(); j++){
  //     //       if(metric.iter_req_stream[iter_id][i] ==
  //     metric.iter_req_stream_fin[iter_id][j]){
  //     //         STREAM_FIN_FLAG = true;
  //     //         break;
  //     //       }
  //     //     }
  //     //     cout << "STREAM_FIN_FLAG: " << STREAM_FIN_FLAG << endl;
  //     //     if(!STREAM_FIN_FLAG){
  //     //     cout << "Cancelling stream #" <<
  //     metric.iter_req_stream[iter_id][i] << endl;
  //     //     sid =
  //     metric.pushStreamInfo.find(metric.iter_req_stream[iter_id][i])->second->sid;
  //     //         fid =
  //     metric.pushStreamInfo.find(metric.iter_req_stream[iter_id][i])->second->fid;
  //     //         layerId =
  //     metric.pushStreamInfo.find(metric.iter_req_stream[iter_id][i])->second->lid;
  //     //
  //     cancelled_request_stream.push_back(metric.iter_req_stream[iter_id][i]);
  //     //       auto ret = nghttp2_submit_rst_stream(session,
  //     NGHTTP2_FLAG_NONE, metric.iter_req_stream[iter_id][i], NGHTTP2_CANCEL);
  //     //       cout << "ret=" << ret << endl;
  //     //     }
  //     // }
  //   }
  // }

  // // update tiles' layers
  //     if(meta.ALG == 5){
  //       if(fin_s_num != req_s_num && sid > 0 && fid > 0){
  //         iter_down_elapsed_time =
  //         std::chrono::duration_cast<std::chrono::milliseconds>(
  //               get_time() - client->timing.connect_end_time).count() -
  //               metric.iter_start_time[iter_id];
  //         cout << "#[ALG5]: " << iter_down_elapsed_time << "," <<
  //         metric.iter_down_time_budget[iter_id] << endl;
  //         if(iter_down_elapsed_time >
  //         0.5*metric.iter_down_time_budget[iter_id] &&
  //         !metric.iter_re_decide[iter_id]){

  //           /* involke only one time for each iteration */
  //           metric.iter_re_decide[iter_id] = true;

  //           /* trung's adaptation */
  //           trung_adaptation(client, session, iter_id,
  //           iter_down_elapsed_time);
  //         }
  //       }
  //     }
  //   }

  if (req->inflater) {
    while (len > 0) {
      const size_t MAX_OUTLEN = 4_k;
      std::array<uint8_t, MAX_OUTLEN> out;
      size_t outlen = MAX_OUTLEN;
      size_t tlen = len;
      int rv =
          nghttp2_gzip_inflate(req->inflater, out.data(), &outlen, data, &tlen);
      if (rv != 0) {
        nghttp2_submit_rst_stream(session, NGHTTP2_FLAG_NONE, stream_id,
                                  NGHTTP2_INTERNAL_ERROR);
        break;
      }

      if (!config.null_out) {
        std::cout.write(reinterpret_cast<const char *>(out.data()), outlen);
      }

      update_html_parser(client, req, out.data(), outlen, 0);
      data += tlen;
      len -= tlen;
    }
    return 0;
  }

  if (!config.null_out) {
    std::cout.write(reinterpret_cast<const char *>(data), len);
  }

  update_html_parser(client, req, data, len, 0);

  return 0;
}
} // namespace

namespace {
ssize_t select_padding_callback(nghttp2_session *session,
                                const nghttp2_frame *frame, size_t max_payload,
                                void *user_data) {
  return std::min(max_payload, frame->hd.length + config.padding);
}
} // namespace

namespace {
void check_response_header(nghttp2_session *session, Request *req) {
  bool gzip = false;

  req->expect_final_response = false;

  auto status_hd = req->get_res_header(http2::HD__STATUS);

  if (!status_hd) {
    nghttp2_submit_rst_stream(session, NGHTTP2_FLAG_NONE, req->stream_id,
                              NGHTTP2_PROTOCOL_ERROR);
    return;
  }

  auto status = http2::parse_http_status_code(StringRef{status_hd->value});
  if (status == -1) {
    nghttp2_submit_rst_stream(session, NGHTTP2_FLAG_NONE, req->stream_id,
                              NGHTTP2_PROTOCOL_ERROR);
    return;
  }

  req->status = status;

  for (auto &nv : req->res_nva) {
    if ("content-encoding" == nv.name) {
      gzip = util::strieq_l("gzip", nv.value) ||
             util::strieq_l("deflate", nv.value);
      continue;
    }
  }

  if (req->status / 100 == 1) {
    if (req->continue_timer && (req->status == 100)) {
      // If the request is waiting for a 100 Continue, complete the handshake.
      req->continue_timer->dispatch_continue();
    }

    req->expect_final_response = true;
    req->status = 0;
    req->res_nva.clear();
    http2::init_hdidx(req->res_hdidx);
    return;
  } else if (req->continue_timer) {
    // A final response stops any pending Expect/Continue handshake.
    req->continue_timer->stop();
  }

  if (gzip) {
    if (!req->inflater) {
      req->init_inflater();
    }
  }
  if (config.get_assets && req->level == 0) {
    if (!req->html_parser) {
      req->init_html_parser();
    }
  }
}
} // namespace

namespace {
int on_begin_headers_callback(nghttp2_session *session,
                              const nghttp2_frame *frame, void *user_data) {
  auto client = get_client(user_data);
  switch (frame->hd.type) {
  case NGHTTP2_HEADERS: {
    auto req = static_cast<Request *>(
        nghttp2_session_get_stream_user_data(session, frame->hd.stream_id));
    if (!req) {
      break;
    }

    switch (frame->headers.cat) {
    case NGHTTP2_HCAT_RESPONSE:
    case NGHTTP2_HCAT_PUSH_RESPONSE:
      req->record_response_start_time();
      break;
    default:
      break;
    }

    break;
  }
  case NGHTTP2_PUSH_PROMISE: {
    auto stream_id = frame->push_promise.promised_stream_id;
    http_parser_url u{};
    // TODO Set pri and level
    nghttp2_priority_spec pri_spec;

    nghttp2_priority_spec_default_init(&pri_spec);

    auto req = make_unique<Request>("", u, nullptr, 0, pri_spec);
    req->stream_id = stream_id;

    nghttp2_session_set_stream_user_data(session, stream_id, req.get());

    client->request_done(req.get());
    req->record_request_start_time();
    client->reqvec.push_back(std::move(req));

    break;
  }
  }
  return 0;
}
} // namespace

namespace {
int on_header_callback(nghttp2_session *session, const nghttp2_frame *frame,
                       const uint8_t *name, size_t namelen,
                       const uint8_t *value, size_t valuelen, uint8_t flags,
                       void *user_data) {
  if (config.verbose) {
    verbose_on_header_callback(session, frame, name, namelen, value, valuelen,
                               flags, user_data);
  }

  switch (frame->hd.type) {
  case NGHTTP2_HEADERS: {
    auto req = static_cast<Request *>(
        nghttp2_session_get_stream_user_data(session, frame->hd.stream_id));

    if (!req) {
      break;
    }

    /* ignore trailer header */
    if (frame->headers.cat == NGHTTP2_HCAT_HEADERS &&
        !req->expect_final_response) {
      break;
    }

    if (req->header_buffer_size + namelen + valuelen > 64_k) {
      nghttp2_submit_rst_stream(session, NGHTTP2_FLAG_NONE, frame->hd.stream_id,
                                NGHTTP2_INTERNAL_ERROR);
      return 0;
    }

    req->header_buffer_size += namelen + valuelen;

    auto token = http2::lookup_token(name, namelen);

    http2::index_header(req->res_hdidx, token, req->res_nva.size());
    http2::add_header(req->res_nva, name, namelen, value, valuelen,
                      flags & NGHTTP2_NV_FLAG_NO_INDEX, token);
    break;
  }
  case NGHTTP2_PUSH_PROMISE: {
    auto req = static_cast<Request *>(nghttp2_session_get_stream_user_data(
        session, frame->push_promise.promised_stream_id));

    if (!req) {
      break;
    }

    if (req->header_buffer_size + namelen + valuelen > 64_k) {
      nghttp2_submit_rst_stream(session, NGHTTP2_FLAG_NONE,
                                frame->push_promise.promised_stream_id,
                                NGHTTP2_INTERNAL_ERROR);
      return 0;
    }

    req->header_buffer_size += namelen + valuelen;

    auto token = http2::lookup_token(name, namelen);

    http2::index_header(req->req_hdidx, token, req->req_nva.size());
    http2::add_header(req->req_nva, name, namelen, value, valuelen,
                      flags & NGHTTP2_NV_FLAG_NO_INDEX, token);
    break;
  }
  }
  return 0;
}
} // namespace

void Dang_adaptation_vr(HttpClient *client, int index);
void UTD2017_adaptation_vr(HttpClient *client, int iter_id);
void MMSP(HttpClient *client, int iter_id);
int send_get_request(HttpClient *client, int index, int face, int br, int dep,
                     int weight);
void trung_adaptation(HttpClient *client, nghttp2_session *session, int iter_id,
                      double iter_down_elapsed_time);

namespace {
int on_frame_recv_callback2(nghttp2_session *session,
                            const nghttp2_frame *frame, void *user_data) {
  int rv = 0;
  int fid, sid, l, i, layerId, my_stream_id, j;
  int *cur_vp_pos;                  /* current viewport postions */
  vector<int> viewport_tile;        /* expected visible tiles */
  vector<int> add_viewport_tile;    /* new visible tiles */
  vector<int> remove_viewport_tile; /* old visible tiles --> terminate */
  vector<int> cancel_stream;
  double iter_down_elapsed_time;
  double iter_down_remain_time;
  nghttp2_stream *stream;
  bool STREAM_FIN_FLAG;
  if (config.verbose) {
    verbose_on_frame_recv_callback(session, frame, user_data);
  }

  auto client = get_client(user_data);
  switch (frame->hd.type) {
  case NGHTTP2_DATA: {
    auto req = static_cast<Request *>(
        nghttp2_session_get_stream_user_data(session, frame->hd.stream_id));
    // cout << "#########DEBUG: " << req << endl;
    if (!req) {
      return 0;
      ;
    }
    if (frame->hd.flags & NGHTTP2_FLAG_END_STREAM) {
      req->record_response_end_time();
      ++client->success;
    }
    //
    if (frame->hd.length >= 0) {
      /* update total download data */
      metric.iter_down_data_frame[iter_id] +=
          frame->hd.length * 8.0 / 1000; // kbit

      /* update per tile downloaded data */
      if (metric.RequestStreamInfo.find(frame->hd.stream_id) !=
          metric.RequestStreamInfo.end()) {

        /* Determine segment/face ids */
        sid = metric.RequestStreamInfo.find(frame->hd.stream_id)->second->sid;
        fid = metric.RequestStreamInfo.find(frame->hd.stream_id)->second->fid;
        layerId =
            metric.RequestStreamInfo.find(frame->hd.stream_id)->second->lid;

        /* updating downloaded data amount of each tile in kbits */
        metric.face_dow_data[sid][fid][layerId] +=
            frame->hd.length * 8.0 / 1000;
        // cout << "data[" << sid << "][" << fid << "][" << layerId << "]=" <<
        // frame->hd.length * 8.0/1000 << endl;

        /* measure elapsed time since the current iteration starts */
        iter_down_elapsed_time =
            std::chrono::duration_cast<std::chrono::milliseconds>(
                get_time() - client->timing.connect_end_time)
                .count() -
            metric.iter_start_time[iter_id];

        /* measure time elapsed since 'connect_end_time */
        auto t_now = std::chrono::duration_cast<std::chrono::microseconds>(
                         get_time() - client->timing.connect_end_time)
                         .count() /
                     1000.0;
        cout << t_now << ", " << player.seg_playback_deadline[sid] << endl;

        // if(meta.ALG == 2){
        if (meta.ALG == 2 || meta.ALG == 3 || meta.ALG == 4 || meta.ALG == 5 ||
            meta.ALG == 6 || meta.ALG == 7 || meta.ALG == 8 || meta.ALG == 9 ||
            meta.ALG == 10 || meta.ALG == 11 || meta.ALG == 12 ||
            meta.ALG == 13) {
          /* terminate late tile */
          if (sid > 0 && fid > 0 &&
              (t_now >= player.seg_playback_deadline[sid])) {
            if (frame->hd.flags & NGHTTP2_FLAG_END_STREAM) {
              req->record_response_end_time();
              ++client->success;
            } else {
              // cout << "Cancelling stream " << frame->hd.stream_id << endl;
              // cout << "Buffer size: " << player.get_cur_buff_size_second() <<
              // endl; cancelled_request_stream.push_back(frame->hd.stream_id);
              // auto ret = nghttp2_submit_rst_stream(session,
              // NGHTTP2_FLAG_NONE, frame->hd.stream_id, NGHTTP2_CANCEL); cout
              // << "ret=" << ret << endl;

              // Find and canncel all active streams //
              for (i = 0; i < metric.iter_req_stream[iter_id].size(); i++) {
                if (std::find(metric.iter_req_stream_fin[iter_id].begin(),
                              metric.iter_req_stream_fin[iter_id].end(),
                              metric.iter_req_stream[iter_id][i]) ==
                    metric.iter_req_stream_fin[iter_id].end()) {
                  cout << "Cancelling stream "
                       << metric.iter_req_stream[iter_id][i] << endl;
                  cout << "Buffer size: " << player.get_cur_buff_size_second()
                       << endl;
                  cancelled_request_stream.push_back(
                      metric.iter_req_stream[iter_id][i]);
                  auto ret = nghttp2_submit_rst_stream(
                      session, NGHTTP2_FLAG_NONE,
                      metric.iter_req_stream[iter_id][i], NGHTTP2_CANCEL);
                  cout << "ret=" << ret << endl;
                }
              }
              //   cout << "STREAM_FIN_FLAG: " << STREAM_FIN_FLAG << endl;
              //   if(!STREAM_FIN_FLAG){
              //   cout << "Cancelling stream #" <<
              //   metric.iter_req_stream[iter_id][i] << endl; sid =
              //   metric.pushStreamInfo.find(metric.iter_req_stream[iter_id][i])->second->sid;
              //       fid =
              //       metric.pushStreamInfo.find(metric.iter_req_stream[iter_id][i])->second->fid;
              //       layerId =
              //       metric.pushStreamInfo.find(metric.iter_req_stream[iter_id][i])->second->lid;
              //     cancelled_request_stream.push_back(metric.iter_req_stream[iter_id][i]);
              //     auto ret = nghttp2_submit_rst_stream(session,
              //     NGHTTP2_FLAG_NONE, metric.iter_req_stream[iter_id][i],
              //     NGHTTP2_CANCEL); cout << "ret=" << ret << endl;
              //   }
              // }
            }
          }
        }

        // update tiles' layers
        if (meta.ALG == 5 || meta.ALG == 6 || meta.ALG == 7 || meta.ALG == 8 ||
            meta.ALG == 9 || meta.ALG == 10 || meta.ALG == 11 ||
            meta.ALG == 12 || meta.ALG == 13) {
          if (fin_s_num != req_s_num && sid > 0 && fid > 0) {
            iter_down_elapsed_time =
                std::chrono::duration_cast<std::chrono::milliseconds>(
                    get_time() - client->timing.connect_end_time)
                    .count() -
                metric.iter_start_time[iter_id];
            // cout << "#[ALG5]: " << iter_down_elapsed_time << "," <<
            // metric.iter_down_time_budget[iter_id] << endl;
            if (iter_down_elapsed_time >
                    0.5 * metric.iter_down_time_budget[iter_id] &&
                !metric.iter_re_decide[iter_id] &&
                cancelled_request_stream.size() == 0) {

              /* involke only one time for each iteration */
              metric.iter_re_decide[iter_id] = true;

              /* trung's adaptation */
              trung_adaptation(client, session, iter_id,
                               iter_down_elapsed_time);
            }
          }
        }
      }
      // cout << endl;
    }

    break;
  }
  case NGHTTP2_HEADERS: {
    auto req = static_cast<Request *>(
        nghttp2_session_get_stream_user_data(session, frame->hd.stream_id));
    // If this is the HTTP Upgrade with OPTIONS method to avoid POST,
    // req is nullptr.
    if (!req) {
      return 0;
    }
    // if(frame -> hd.stream_id % 2 == 0) {
    //   //cout << req -> make_reqpath() << "\t" <<dang_get_face_from_uri (req
    //   -> make_reqpath())<<endl;
    //   //face_stream_id
    //   face_stream_id [dang_get_face_from_uri (req -> make_reqpath())] = frame
    //   -> hd.stream_id; if (dang_get_face_from_uri (req -> make_reqpath()) ==
    //   0) {
    //     start_measure_thrp =
    //     std::chrono::duration_cast<std::chrono::milliseconds>(
    //                     req->timing.response_start_time -
    //                     client->timing.connect_end_time).count();
    //     cout << "Start measure time " << start_measure_thrp << endl;
    //   }
    // }
    // if(dang_get_seg_from_uri(req->make_reqpath())==seg_terminate)
    //   decide_cancel=true;
    // else decide_cancel =false;
    switch (frame->headers.cat) {
    case NGHTTP2_HCAT_RESPONSE:
    case NGHTTP2_HCAT_PUSH_RESPONSE:
      check_response_header(session, req);
      // cout << "abc"<<endl;
      break;
    case NGHTTP2_HCAT_HEADERS:
      if (req->expect_final_response) {
        check_response_header(session, req);
        break;
      }
      if ((frame->hd.flags & NGHTTP2_FLAG_END_STREAM) == 0) {
        nghttp2_submit_rst_stream(session, NGHTTP2_FLAG_NONE,
                                  frame->hd.stream_id, NGHTTP2_PROTOCOL_ERROR);
        cout << "dcm0";
        return 0;
      }
      break;
    default:
      assert(0);
    }

    if (frame->hd.flags & NGHTTP2_FLAG_END_STREAM) {
      req->record_response_end_time();
      ++client->success;
    }

    break;
  }
  case NGHTTP2_SETTINGS:
    if ((frame->hd.flags & NGHTTP2_FLAG_ACK) == 0) {
      break;
    }
    ev_timer_stop(client->loop, &client->settings_timer);
    break;
  case NGHTTP2_PUSH_PROMISE: {
    auto req = static_cast<Request *>(nghttp2_session_get_stream_user_data(
        session, frame->push_promise.promised_stream_id));
    if (!req) {
      break;
    }
    /* determine push_stream of each request */
    int stream_id, sid, fid, layerId;
    StreamInfo *streamInfo;
    // stream_id = frame->push_promise.hd.stream_id;
    // if(metric.stream_id_2_sid.find(stream_id) !=
    // metric.stream_id_2_sid.end()){
    //   sid = metric.stream_id_2_sid.find(stream_id)->second;
    //   fid = metric.stream_id_2_fid.find(stream_id)->second;
    //   layerId = metric.stream_id_2_lid.find(stream_id)->second;
    //   metric.face_push_stream_id[sid][fid][layerId] =
    //   frame->push_promise.promised_stream_id;
    //   metric.push_stream_id_2_sid.insert(std::pair<int, int>
    //   (metric.face_push_stream_id[sid][fid][layerId], sid));
    //   metric.push_stream_id_2_fid.insert(std::pair<int, int>
    //   (metric.face_push_stream_id[sid][fid][layerId], fid));
    //   metric.push_stream_id_2_lid.insert(std::pair<int, int>
    //   (metric.face_push_stream_id[sid][fid][layerId], layerId));
    // }
    stream_id = frame->push_promise.hd.stream_id;
    if (metric.RequestStreamInfo.find(stream_id) !=
        metric.RequestStreamInfo.end()) {
      sid = metric.RequestStreamInfo.find(stream_id)->second->sid;
      fid = metric.RequestStreamInfo.find(stream_id)->second->fid;
      layerId = metric.RequestStreamInfo.find(stream_id)->second->lid;

      streamInfo = new StreamInfo(sid, fid, layerId);

      metric.iter_push_stream[iter_id].push_back(
          frame->push_promise.promised_stream_id);
      metric.face_push_stream_id[sid][fid][layerId] =
          frame->push_promise.promised_stream_id;
      metric.pushStreamInfo.insert(std::pair<int, StreamInfo *>(
          metric.face_push_stream_id[sid][fid][layerId], streamInfo));

      metric.push_stream_id_2_sid.insert(std::pair<int, int>(
          metric.face_push_stream_id[sid][fid][layerId], sid));
      metric.push_stream_id_2_fid.insert(std::pair<int, int>(
          metric.face_push_stream_id[sid][fid][layerId], fid));
      metric.push_stream_id_2_lid.insert(std::pair<int, int>(
          metric.face_push_stream_id[sid][fid][layerId], layerId));

      metric.face_dow_data[sid][fid][layerId] = 0;
    }
    // cout << frame->push_promise.promised_stream_id << '\t' << face_stream_id
    // [0] <<endl; if (frame->push_promise.promised_stream_id ==
    // face_stream_id[0]) {

    // }
    req->header_buffer_size = 0;

    auto scheme = req->get_req_header(http2::HD__SCHEME);
    auto authority = req->get_req_header(http2::HD__AUTHORITY);
    auto path = req->get_req_header(http2::HD__PATH);

    if (!authority) {
      authority = req->get_req_header(http2::HD_HOST);
    }

    // libnghttp2 guarantees :scheme, :method, :path and (:authority |
    // host) exist and non-empty.
    if (path->value[0] != '/') {
      nghttp2_submit_rst_stream(session, NGHTTP2_FLAG_NONE,
                                frame->push_promise.promised_stream_id,
                                NGHTTP2_PROTOCOL_ERROR);
      cout << "dcm1";
      break;
    }
    std::string uri = scheme->value;
    uri += "://";
    uri += authority->value;
    uri += path->value;
    http_parser_url u{};
    if (http_parser_parse_url(uri.c_str(), uri.size(), 0, &u) != 0) {
      nghttp2_submit_rst_stream(session, NGHTTP2_FLAG_NONE,
                                frame->push_promise.promised_stream_id,
                                NGHTTP2_PROTOCOL_ERROR);
      cout << "dcm2";
      break;
    }
    req->uri = uri;
    req->u = u;

    if (client->path_cache.count(uri)) {
      nghttp2_submit_rst_stream(session, NGHTTP2_FLAG_NONE,
                                frame->push_promise.promised_stream_id,
                                NGHTTP2_CANCEL);
      cout << "dcm3";
      break;
    }

    if (config.multiply == 1) {
      client->path_cache.insert(uri);
    }

    break;
  }
  }
  return rv;
}
} // namespace

namespace {
int before_frame_send_callback(nghttp2_session *session,
                               const nghttp2_frame *frame, void *user_data) {
  if (frame->hd.type != NGHTTP2_HEADERS ||
      frame->headers.cat != NGHTTP2_HCAT_REQUEST) {
    return 0;
  }
  auto req = static_cast<Request *>(
      nghttp2_session_get_stream_user_data(session, frame->hd.stream_id));
  assert(req);
  req->record_request_start_time();
  return 0;
}

} // namespace

namespace {
int on_frame_send_callback(nghttp2_session *session, const nghttp2_frame *frame,
                           void *user_data) {
  if (config.verbose) {
    verbose_on_frame_send_callback(session, frame, user_data);
  }

  if (frame->hd.type != NGHTTP2_HEADERS ||
      frame->headers.cat != NGHTTP2_HCAT_REQUEST) {
    return 0;
  }

  auto req = static_cast<Request *>(
      nghttp2_session_get_stream_user_data(session, frame->hd.stream_id));
  if (!req) {
    return 0;
  }

  // If this request is using Expect/Continue, start its ContinueTimer.
  if (req->continue_timer) {
    req->continue_timer->start();
  }

  return 0;
}
} // namespace

namespace {
int on_frame_not_send_callback(nghttp2_session *session,
                               const nghttp2_frame *frame, int lib_error_code,
                               void *user_data) {
  if (frame->hd.type != NGHTTP2_HEADERS ||
      frame->headers.cat != NGHTTP2_HCAT_REQUEST) {
    return 0;
  }

  auto req = static_cast<Request *>(
      nghttp2_session_get_stream_user_data(session, frame->hd.stream_id));
  if (!req) {
    return 0;
  }

  std::cerr << "[ERROR] request " << req->uri
            << " failed: " << nghttp2_strerror(lib_error_code) << std::endl;

  return 0;
}
} // namespace
void send_request_handle(struct ev_loop *loop, ev_timer *w, int revents) {
  // cout << "#[send_request_handle] "<<endl;
  if (isFirstTime) {
    cnt_timer++;
    if (cnt_timer == 1000) {
      cout << "Stop timer" << endl;
      ev_timer_stop(loop, &tev);
      isFirstTime = false;
      cnt_timer = 0;
      return;
    }
    return;
  }
  auto client = static_cast<HttpClient *>(ev_userdata(loop));
  int curr_time = std::chrono::duration_cast<std::chrono::milliseconds>(
                      get_time() - client->timing.connect_end_time)
                      .count();
  // cout << "Curr time: " << curr_time / 1000.0 << endl;
  cnt_timer++;
  if (cnt_timer == max_timer || frame_buffer[0] <= initial_buffer) {
    cnt_timer = 0;
    // Dang_adaptation_vr (client, 0);
    cnt_face = 0;
    ev_timer_stop(loop, &tev);
  }
}

// Hung: given the instant throughput, compute the maximum adapted bitrate
// In this method, the maximum adapted bitrate is considered as a rate candidate
//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------

// Hung: rebufferring phase

// Hung: change the bitrate without refusing the stream
// Note that we disable adaptation until a new segment is fully received

//-------------------------------------------------------------------------------
int max_rate_less_than_thrp(double thrp) {
  // thrp = (1-safety_margin)*thrp;

  for (int i = dang_rate_set_orginal.size() - 1; i >= 0; i--) {
    if (dang_rate_set_orginal.at(i) <= thrp)
      return dang_rate_set_orginal.at(i);
  }
  return dang_rate_set_orginal.at(0);
}

// void dang_req_vod_rate(HttpClient *client ,int index, bool submit=true) {
//   int num1, num2, num3;
//   int weight, ver_id;
//   last_region = dang_K;
//   int fid;
//   int RATE_ALOC_ALG = 2;
//   double totalBR, usedBR;
//   switch(RATE_ALOC_ALG){
//     case 1:
//       for(fid= 0 ; fid < dang_K ; fid++){

//         /* get request id of each stream */
//         curr_request_stream_id += 2;

//         /* assign weight */
//         weight = 20;
//         nghttp2_priority_spec_init(&dang_pri_spec, 11, weight, 0);

//         /* assign bitrate */
//         dang_face_rate[fid] = dang_rate_set_orginal[0];
//         rate_string = to_string (dang_face_rate [fid]);
//         if
//         (client->add_request(dang_uri+"/face"+to_string(fid)+"/bitrate="+rate_string+"/num=1",
//               dang_data_prd, dang_data_length, dang_pri_spec)) {

//           /* send request */
//           if(submit)
//             submit_request(client, dang_headers,
//             client->reqvec.back().get());
//         }
//         for (int i = 0; i < num_of_tile; i++)
//           data_frame_compenstation [i]= 0;

//         /* record */
//         // metric.face_stream_ID[index][fid] = curr_request_stream_id;
//         // metric.face_br[index][fid] = dang_face_rate[fid];
//         // metric.face_ini_weight[index][fid] = weight;
//         //
//         metric.stream_id_2_sid.insert(std::pair<int,int>(curr_request_stream_id,
//         index));
//         //
//         metric.stream_id_2_fid.insert(std::pair<int,int>(curr_request_stream_id,
//         fid));

//         std::cout << "[ADAPTATION] bitrate for face "<< fid << " rate : " <<
//         rate_string
//           << std::endl << std::endl;
//       }
//       break;
//     case 2:
//       /* select bitrate */
//       totalBR = 0;
//       usedBR = 0;

//       /* select the lowest bitrate for all faces */
//       for(fid=0; fid < meta.NO_FACE; fid ++){
//         // metric.face_br[index][fid] = dang_rate_set_orginal[0];
//         usedBR += dang_rate_set_orginal[0];
//       }
//       cout << "UsedBR: " << usedBR << ", remain: " << (totalBR - usedBR) <<
//       endl;

//        select the highest possible bitrate for face 0
//       if(usedBR < totalBR){
//         for(ver_id = 1; ver_id < NO_VER; ver_id ++){
//           cout << "ver: " << ver_id << ", " << dang_rate_set_orginal[ver_id]
//           << endl; if(dang_rate_set_orginal[ver_id] -
//           dang_rate_set_orginal[0] > (totalBR - usedBR))
//             break;
//         }
//         if(ver_id > 1){
//           // usedBR += (dang_rate_set_orginal[ver_id-1] -
//           metric.face_br[index][0]);
//           // metric.face_br[index][0] = dang_rate_set_orginal[ver_id - 1];
//         }
//       }

//       /* select the highest possible bitrate for other faces */

//       /* Assign tiles' priorities proportional to faces' bitrates */
//       for(fid=0; fid < meta.NO_FACE; fid++){
//         metric.face_ini_weight[index][fid] = (int) (120 *
//         metric.face_br[index][fid] * 1.0 / usedBR); cout << "fid: " << fid <<
//         ", " << metric.face_br[index][fid] << ", " <<
//         metric.face_ini_weight[index][fid] << endl; curr_request_stream_id +=
//         2;
//         // metric.face_stream_ID[index][fid] = curr_request_stream_id;
//         //
//         metric.stream_id_2_sid.insert(std::pair<int,int>(curr_request_stream_id,
//         index));
//         //
//         metric.stream_id_2_fid.insert(std::pair<int,int>(curr_request_stream_id,
//         fid));

//         /* create request */
//         nghttp2_priority_spec_init(&dang_pri_spec, 11,
//         metric.face_ini_weight[index][fid], 0); if
//         (client->add_request(dang_uri+"/face"+to_string(fid)+"/bitrate="+to_string(metric.face_br[index][fid])+"/num=1",
//         dang_data_prd, dang_data_length, dang_pri_spec)) {
//             /* send request */
//             if(submit)
//               submit_request(client, dang_headers,
//               client->reqvec.back().get());
//         }
//       }
//       break;
//   }
//   is_sent_cancel = false;
//   is_face_0_finished = false;
//   time_start_send_request =
//   std::chrono::duration_cast<std::chrono::milliseconds>(
//                      get_time() - client->timing.connect_end_time).count();
//   finish_curr_seg = false;
//   late_send_request = false;
//   client->signal_write();

// }

void dang_change_rate(HttpClient *client, int new_rate, int video) {
  // if(dang_adapted_rate [video-1]== new_rate) return;
  // else dang_adapted_rate[video-1] = new_rate;

  std::string rate_string = std::to_string(new_rate);

  if (client->add_request(dang_uri + "/change" + to_string(video) + "/" +
                              rate_string,
                          dang_data_prd, dang_data_length, dang_pri_spec)) {
    submit_request(client, dang_headers, client->reqvec.back().get());
  }

  std::cout << "[ADAPTATION] bitrate " << rate_string << " of tile " << video
            << std::endl
            << std::endl;

  client->signal_write();
}
void display_timer_handle(struct ev_loop *loop, ev_timer *w, int revents) {
  auto client = static_cast<HttpClient *>(ev_userdata(loop));
  cout << "Decoded [ "
       << std::chrono::duration_cast<std::chrono::milliseconds>(
              get_time() - client->timing.connect_end_time)
              .count()
       << "]";
  for (int i = 0; i < num_of_tile; i++) {
    cout << " Face " << i << ": " << frame_buffer[i] << " frames" << '\t';
  }
  cout << endl;
  GOP_buffer_file << std::chrono::duration_cast<std::chrono::milliseconds>(
                         get_time() - client->timing.connect_end_time)
                             .count() /
                         1000.0
                  << '\t';
  for (int i = 0; i < num_of_tile; i++) {
    if (i >= dang_K && frame_buffer[i] <= 0)
      GOP_buffer_file << "x" << '\t';

    else {
      if (frame_buffer[i] < 0)
        GOP_buffer_file << 0 << '\t';
      else
        GOP_buffer_file << frame_buffer[i] / GOP_size << '\t';
    }
  }
  GOP_buffer_file << endl;

  bool stop_timer = false;
  // for (int i = 0; i < frame_buffer [i] ; i ++) {
  //   if (frame_buffer [i] > 0)
  //     stop_timer = false;
  // }
  if (frame_buffer[0] <= -100000) {
    start_time_rebuff = std::chrono::duration_cast<std::chrono::milliseconds>(
                            get_time() - client->timing.connect_end_time)
                            .count();
    dang_buffering2 = true;
    ev_timer_stop(loop, &dang_tev);
  } else {
    for (int i = 0; i < num_of_tile; i++) {
      if (i < dang_K || (i >= dang_K && frame_buffer[i] > 0))
        frame_buffer[i] -= GOP_size;
      // else frame_buffer [i] = 0;
    }
  }
  if (closed_connection) {
    bool stop_timer = true;
    for (int i = 0; i < num_of_tile; i++) {
      if (frame_buffer[i] > 0)
        stop_timer = false;
      // else frame_buffer [i] = 0;
    }
    if (stop_timer) {
      ev_timer_stop(loop, &dang_tev);
      GOP_buffer_file.close();
    }
  }
}
void setSmoothedBW() {
  double sigma = 0.125;
  if (dang_segment_rate.size() == 1) {
    smoothedBW = dang_inst_thrp;
  } else {
    smoothedBW = dang_inst_thrp * sigma + smoothedBW * (1 - sigma);
  }
}
int send_get_request(HttpClient *client, int index, int fid, int br, int dep,
                     int w) {
  /* assign  weight and  dependency to this stream */
  nghttp2_priority_spec_init(&dang_pri_spec, dep, w, 0);

  /* send request */
  if (client->add_request(dang_uri + "/face" + to_string(fid) +
                              "/bitrate=" + to_string(br) + "/num=1",
                          dang_data_prd, dang_data_length, dang_pri_spec)) {

    submit_request(client, dang_headers, client->reqvec.back().get());
  }
  cur_req_stream_id += 2;
  return cur_req_stream_id;
}

void trung_adaptation(HttpClient *client, nghttp2_session *session, int iter_id,
                      double iter_down_elapsed_time) {
  /* predictViewport */
  vector<int> C;
  vector<int> D;
  vector<int> E;
  vector<int> viewport_tile;
  vector<int> viewport_tile_estimate;
  int i, j, l, sid, fid, layerId, total_br_remain_stream = 0, EL_br_tmp = 0;
  bool STREAM_FIN_FLAG;
  StreamInfo *streamInfo;

  cout << "######### trung_adaptation ######" << endl;
  cout << "######### iter_id = " << iter_id << "######" << endl;
  cout << "Down. time budget: " << metric.iter_down_time_budget[iter_id]
       << endl;
  /* invoke only 1 time */
  // metric.iter_re_decide[iter_id] = true;

  /* predictViewport */
  int *cur_vp_pos = player.htrace->get_vp_pos(player.get_cur_play_time());
  viewport_tile = player.vmask->get_visible_tile(cur_vp_pos);

  for (i = 0; i < viewport_tile.size(); i++) {
    cout << viewport_tile[i] << ",";
    if (!IsVectorElement(metric.iter_tile_list[iter_id], viewport_tile[i]))
      E.push_back(viewport_tile[i]);
  }
  cout << endl;

  for (i = 0; i < metric.iter_tile_list[iter_id].size(); i++) {
    cout << metric.iter_tile_list[iter_id][i] << ",";
    if (!IsVectorElement(viewport_tile, metric.iter_tile_list[iter_id][i]))
      D.push_back(metric.iter_tile_list[iter_id][i]);
  }
  cout << endl;

  /* find 'remove' visible tiles */
  // set_difference(metric.iter_tile_list[iter_id].begin(),
  // metric.iter_tile_list[iter_id].end(), viewport_tile.begin(),
  // viewport_tile.end(), std::inserter(D, D.begin()));

  /* find 'new' visible tiles */
  // set_difference(viewport_tile.begin(), viewport_tile.end(),
  // metric.iter_tile_list[iter_id].begin(),
  // metric.iter_tile_list[iter_id].end(), std::inserter(E, E.begin()));

  cout << "Old visible tiles: " << D.size() << endl;

  for (i = 0; i < D.size(); i++)
    cout << D[i] << ",";
  cout << endl;

  cout << "New visible tiles: " << E.size() << endl;
  for (i = 0; i < E.size(); i++)
    cout << E[i] << ",";
  cout << endl;

  /////////////////////////////////////////////////////////////////
  ///// cancel active push streams of old visible tiles ///////////
  /////////////////////////////////////////////////////////////////
  if (E.size() > 0) {
    cout << "============ iter_req_stream_num: "
         << metric.iter_req_stream[iter_id].size() << endl;
    for (i = 0; i < metric.iter_req_stream[iter_id].size(); i++) {
      sid = metric.RequestStreamInfo.find(metric.iter_req_stream[iter_id][i])
                ->second->sid;
      fid = metric.RequestStreamInfo.find(metric.iter_req_stream[iter_id][i])
                ->second->fid;
      layerId =
          metric.RequestStreamInfo.find(metric.iter_req_stream[iter_id][i])
              ->second->lid;
      cout << "============ " << metric.iter_req_stream[iter_id][i] << ","
           << sid << "," << fid << "," << layerId << endl;
      cout << "============ " << metric.iter_req_stream_fin[iter_id].size()
           << endl;

      if (std::find(metric.iter_req_stream_fin[iter_id].begin(),
                    metric.iter_req_stream_fin[iter_id].end(),
                    metric.iter_req_stream[iter_id][i]) !=
          metric.iter_req_stream_fin[iter_id].end()) {
        STREAM_FIN_FLAG = true;
      } else {
        STREAM_FIN_FLAG = false;
      }
      // for(j=0; j < metric.iter_req_stream_fin[iter_id].size(); j++){
      //   cout << "============== " << metric.iter_req_stream_fin[iter_id][j]
      //   << "," << metric.iter_req_stream[iter_id][i] << endl;
      //   if(metric.iter_req_stream[iter_id][i] ==
      //   metric.iter_push_stream_fin[iter_id][j]){
      //     cout << "============= TRUE" << endl;
      //     STREAM_FIN_FLAG = true;
      //     break;
      //   }
      // }
      cout << "STREAM_FIN_FLAG: " << STREAM_FIN_FLAG << ", sid=" << sid << ","
           << fid << "," << layerId << endl;
      if (!STREAM_FIN_FLAG) {
        cout << "###check stream for cancelling #\n";
        cout << IsVectorElement(D, fid) << "\n";
        cout << metric.face_dow_data[sid][fid][layerId] << "\n";
        if (IsVectorElement(D, fid) &&
            metric.face_dow_data[sid][fid][layerId] == 0) {
          cout << "Cancelling stream #" << fid << "\n";
          //  << metric.iter_push_stream[iter_id][i] << ", sid=" << sid << "," <<
          // fid << "," << layerId  << endl;
          // cancelled_request_stream.push_back(metric.face_stream_ID[sid][fid][layerId]);
          // metric.face_ini_weight[sid][fid][layerId] = 30;
          // nghttp2_priority_spec_init(
              // &trung_pri_spec, metric.face_stream_ID[EL_seg_id][fid][layerId],
              // metric.face_ini_weight[EL_seg_id][fid][layerId], 0);
          // nghttp2_submit_priority(session, NGHTTP2_FLAG_NONE,
                                  // metric.face_stream_ID[sid][fid][layerId],
                                  // &trung_pri_spec);
          // nghttp2_submit_rst_stream(session, NGHTTP2_FLAG_NONE,
          // metric.iter_push_stream[iter_id][i], NGHTTP2_CANCEL);
          nghttp2_submit_rst_stream(session, NGHTTP2_FLAG_NONE,
          metric.face_stream_ID[sid][fid][layerId], NGHTTP2_CANCEL);
          total_br_remain_stream += meta.enha_layer_br[layerId];
        }
      }
    }
  }
  /////////////////////////////////////////////////////////////////
  /// Decide versions and request EL of new visible tile ///////////
  /////////////////////////////////////////////////////////////////

  // find versions of new tiles
  for (l = 0; l < meta.EL_VER_NUM; l++) {
    if (((EL_br_tmp + meta.enha_layer_br[l]) * E.size() +
         total_br_remain_stream) *
            meta.SD >
        metric.iter_est_thrp[iter_id] *
            (metric.iter_down_time_budget[iter_id] - iter_down_elapsed_time) /
            1000.0)
      break;
    EL_br_tmp += meta.enha_layer_br[l];
  }
  cout << "l=" << l << endl;

  /* send get requests */
  /*
   */
  if (l > 0) {
    for (i = 0; i < E.size(); i++) {
      for (layerId = 1; layerId <= l; layerId++) {

        // assign bitrate and weights
        metric.face_br[EL_seg_id][E[i]][layerId] =
            meta.enha_layer_br[layerId - 1];
        cout << meta.enha_layer_br[layerId] << endl;

        metric.face_ini_weight[EL_seg_id][E[i]][layerId] = 10;

        // send request
        metric.face_stream_ID[EL_seg_id][E[i]][layerId] = send_get_request(
            client, EL_seg_id, E[i], metric.face_br[EL_seg_id][E[i]][layerId],
            0, metric.face_ini_weight[EL_seg_id][E[i]][layerId]);
        metric.iter_req_stream[iter_id].push_back(
            metric.face_stream_ID[EL_seg_id][E[i]][layerId]);

        // map stream_id -> (index, faceId, layerId)
        streamInfo = new StreamInfo(EL_seg_id, E[i], layerId);
        metric.RequestStreamInfo.insert(std::pair<int, StreamInfo *>(
            metric.face_stream_ID[EL_seg_id][E[i]][layerId], streamInfo));
        //
        req_s_num++;
        //
        cout << "# Requesting new req_stream_id: "
             << metric.face_stream_ID[EL_seg_id][E[i]][layerId] << ","
             << metric.face_br[EL_seg_id][E[i]][layerId] << "," << E[i] << ","
             << layerId << endl;
      }
      metric.iter_tile_list[iter_id].push_back(E[i]);
    }
  }
};
void MMSP(HttpClient *client, int iter_id) {
  int l, i, layerId, j, k, ID;
  int *cur_vp_pos;
  int *tile_pixel;
  int *tile_pixel_2;
  vector<int> pri;
  vector<int> viewport_tile, adj_viewport_tile;
  double EL_br_tmp = 0;
  int sum_tile_br;
  StreamInfo *streamInfo;
  int max_temp, min_temp;

  // cout << "#[MMSP]: iter_id="  << iter_id << "," << (player.playing_seg_id +
  // 1) << endl;
  metric.iter_start_time[iter_id] =
      std::chrono::duration_cast<std::chrono::microseconds>(
          get_time() - client->timing.connect_end_time)
          .count() /
      1000.0;

  /* reset cancelled stream */
  cancelled_request_stream.clear();

  /* estimate available bandwidth for this iteration */
  metric.est_iter_thrp(iter_id);
  if (meta.ALG == 11)
    metric.cal_smooth_bitrate(iter_id, 500);
  else
    metric.cal_smooth_bitrate(iter_id, 200);

  cout << "#[MMSP]: iter_id=" << iter_id << "," << (player.playing_seg_id + 1)
       << "est_thrp=" << metric.iter_est_thrp[iter_id] << ","
       << player.get_cur_buff_size_second()
       << ",LAST_ITER_MASK: " << LAST_ITER_MASK << endl;
  req_s_num = 0;

  // set buffering mode
  if (player.get_cur_buff_size_second() >= meta.BASE_BUFF_THRES &&
      INI_BUFFERING) {
    INI_BUFFERING = false;
    cout << "=========MMSP initial buffering finsihed" << endl;
  }

  if (player.get_cur_buff_size_second() < meta.BASE_BUFF_THRES / 2 &&
      !RE_BUFFERRING && !INI_BUFFERING) {
    RE_BUFFERRING = true;
  }

  if (player.get_cur_buff_size_second() >= meta.BASE_BUFF_THRES &&
      !INI_BUFFERING && RE_BUFFERRING) {
    RE_BUFFERRING = false;
  }

  if (BL_seg_id < meta.NO_SEG) {

    // if ((INI_BUFFERING &&
    //      player.get_cur_buff_size_second() < (meta.BASE_BUFF_THRES)) ||
    //     (!INI_BUFFERING && !RE_BUFFERRING &&
    //      player.get_cur_buff_size_second() < (meta.BASE_BUFF_THRES) &&
    //      LAST_ITER_MASK == 1) ||
    //     (!INI_BUFFERING && RE_BUFFERRING)) {

    if ((INI_BUFFERING &&
         player.get_cur_buff_size_second() < (meta.BASE_BUFF_THRES * 15)) ||
        (!INI_BUFFERING && !RE_BUFFERRING &&
         player.get_cur_buff_size_second() < (meta.BASE_BUFF_THRES * 15) &&
         LAST_ITER_MASK == 1) ||
        (!INI_BUFFERING && RE_BUFFERRING)) {
      // if((INI_BUFFERING && player.get_cur_buff_size_second() <
      // meta.BASE_BUFF_THRES) || (!INI_BUFFERING &&
      // player.get_cur_buff_size_second() < meta.BASE_BUFF_THRES - 1)){
      /* next base layer segment ID */
      metric.iter_tile_version[iter_id] = 0;
      metric.iter_down_time_budget[iter_id] =
          player.seg_playback_deadline[BL_seg_id] -
          (std::chrono::duration_cast<std::chrono::microseconds>(
               std::chrono::steady_clock::now() -
               client->timing.connect_end_time)
               .count() /
           1000.0);

      /* Download base layer of the current segment */
      metric.face_br[BL_seg_id][0][0] = meta.base_layer_br;
      metric.face_ini_weight[BL_seg_id][0][0] = 20;

      /* send request */
      metric.face_stream_ID[BL_seg_id][0][0] = send_get_request(
          client, BL_seg_id, 0, metric.face_br[BL_seg_id][0][0], 0,
          metric.face_ini_weight[BL_seg_id][0][0]);
      cout << "#[MMSP] req_stream_id: "
           << metric.face_stream_ID[BL_seg_id][0][0] << "," << BL_seg_id
           << endl;

      /* map stream_id -> (index, faceid) */
      streamInfo = new StreamInfo(BL_seg_id, 0, 0);
      metric.RequestStreamInfo.insert(std::pair<int, StreamInfo *>(
          metric.face_stream_ID[BL_seg_id][0][0], streamInfo));
      metric.iter_req_stream[iter_id].push_back(
          metric.face_stream_ID[BL_seg_id][0][0]);

      /* update status */
      req_s_num = 1;
      fin_s_num = 0;

      LAST_ITER_MASK = 0;
    }
  }
  if (req_s_num == 0) {
    /* decide and request EL layer chunks of tiles of next played segment */
    if (player.playing_seg_id == meta.NO_SEG - 1) {
      return;
    }

    /* headNext(): get next playing segment */
    EL_seg_id = player.playing_seg_id + 1;
    // if(!metric.seg_EL_download[EL_seg_id] && BL_seg_id < meta.NO_SEG){
    if (!metric.seg_EL_download[EL_seg_id] && EL_seg_id < meta.NO_SEG) {

      LAST_ITER_MASK = 1;
      /* predictViewport */
      cur_vp_pos = player.htrace->get_vp_pos(player.get_cur_play_time());

      // known viewports first frame
      // cur_vp_pos = player.htrace->get_vp_pos(EL_seg_id * player.SD);

      // known viewports middle frame
      // cur_vp_pos = player.htrace->get_vp_pos(EL_seg_id * player.SD +
      // player.SD/2);

      viewport_tile = player.vmask->get_visible_tile(cur_vp_pos);
      // adj_viewport_tile = player.vmask->get_adjacent_tile(cur_vp_pos);
      // cout << "##############Adjacent tiles:" << endl;
      // for(int i=0; i < adj_viewport_tile.size(); i++)
      // 	cout << adj_viewport_tile[i] << ",";
      // cout << endl;

      player.seg_est_vp[EL_seg_id] = cur_vp_pos;

      // // known viewport #2: t0 + SD/2
      // viewport_tile =
      // player.vmask->get_visible_tile(player.htrace->get_vp_pos(EL_seg_id *
      // player.SD + player.SD/2.0));

      cout << "#[MMSP]: " << cur_vp_pos[0] << ", " << cur_vp_pos[1] << ", "
           << player.get_cur_play_time() << endl;

      if (meta.ALG == 4) {
        metric.iter_down_time_budget[iter_id] =
            player.seg_playback_deadline[EL_seg_id] -
            (std::chrono::duration_cast<std::chrono::microseconds>(
                 std::chrono::steady_clock::now() -
                 client->timing.connect_end_time)
                 .count() /
             1000.0);
        cout << "ALG4: " << metric.iter_down_time_budget[iter_id] << endl;
        auto start = chrono::steady_clock::now();
        // layers of tiles(loop variable)
        int *tile_layer = new int[10];

        // selected layers of tiles
        int *selected_tile_layer = new int[viewport_tile.size()];

        // temporal sum at each iteration
        double *tmp_sum = new double[10];
        int cnt = 0;
        double max_vp_psnr = 0, vp_psnr, VIEWPORT_SIZE = 960.0 * 960.0;
        // double enha_layer_psnr[] = {35.0, 37.0, 39.0};
        double enha_layer_psnr[] = {125.0, 325.0, 725.0};

        tile_pixel = player.vmask->get_pixel(cur_vp_pos);

        // nested `for` loops to iterate over all possible combinations

        // 1st viewport tile
        for (tile_layer[0] = 0; tile_layer[0] < 3; tile_layer[0]++) {

          // break if all tiles have been considered
          cnt = 1;
          if (cnt > viewport_tile.size())
            break;

          // break if bandwidth is insuficient
          tmp_sum[0] = 0;
          if (tile_layer[0] > 0) {
            for (i = 1; i <= tile_layer[0]; i++)
              tmp_sum[0] += meta.enha_layer_br[i - 1];
          }
          if (tmp_sum[0] + meta.base_layer_br >
              metric.iter_est_thrp[iter_id] *
                  metric.iter_down_time_budget[iter_id] / (meta.SD * 1000))
            break;

          // update optimal solution if all tiles have been looped
          if (cnt == viewport_tile.size()) {
            vp_psnr = 0;
            for (i = 0; i < cnt; i++) {
              selected_tile_layer[i] = tile_layer[i];
              vp_psnr += enha_layer_psnr[tile_layer[i]] *
                         tile_pixel[viewport_tile[i] - 1] / VIEWPORT_SIZE;
              if (vp_psnr > max_vp_psnr) {
                max_vp_psnr = vp_psnr;
              }
            }
          }

          // 2nd tile
          for (tile_layer[1] = 0; tile_layer[1] < 3; tile_layer[1]++) {

            // break if all tiles have been considered
            cnt = 2;
            if (cnt > viewport_tile.size())
              break;

            // break if bandwidth is insuficient
            tmp_sum[1] = 0;
            if (tile_layer[1] > 0) {
              for (i = 1; i <= tile_layer[1]; i++)
                tmp_sum[1] += meta.enha_layer_br[i - 1];
            }
            if (tmp_sum[0] + tmp_sum[1] + meta.base_layer_br >
                metric.iter_est_thrp[iter_id] *
                    metric.iter_down_time_budget[iter_id] / (meta.SD * 1000))
              break;

            // update optimal solution if all tiles have been looped
            if (cnt == viewport_tile.size()) {
              vp_psnr = 0;
              for (i = 0; i < cnt; i++) {
                selected_tile_layer[i] = tile_layer[i];
                vp_psnr += enha_layer_psnr[tile_layer[i]] *
                           tile_pixel[viewport_tile[i] - 1] / VIEWPORT_SIZE;
                if (vp_psnr > max_vp_psnr) {
                  max_vp_psnr = vp_psnr;
                }
              }
            }

            // 3rd tile
            for (tile_layer[2] = 0; tile_layer[2] < 3; tile_layer[2]++) {

              // break if all tiles have been considered
              cnt = 3;
              if (cnt > viewport_tile.size())
                break;

              // break if bandwidth is insuficient
              tmp_sum[2] = 0;
              if (tile_layer[2] > 0) {
                for (i = 1; i <= tile_layer[2]; i++)
                  tmp_sum[2] += meta.enha_layer_br[i - 1];
              }
              if (tmp_sum[0] + tmp_sum[1] + tmp_sum[2] + meta.base_layer_br >
                  metric.iter_est_thrp[iter_id] *
                      metric.iter_down_time_budget[iter_id] / (meta.SD * 1000))
                break;

              // update optimal solution if all tiles have been looped
              if (cnt == viewport_tile.size()) {
                vp_psnr = 0;
                for (i = 0; i < cnt; i++) {
                  selected_tile_layer[i] = tile_layer[i];
                  vp_psnr += enha_layer_psnr[tile_layer[i]] *
                             tile_pixel[viewport_tile[i] - 1] / VIEWPORT_SIZE;
                  if (vp_psnr > max_vp_psnr) {
                    max_vp_psnr = vp_psnr;
                  }
                }
              }

              // 4th tile
              for (tile_layer[3] = 0; tile_layer[3] < 3; tile_layer[3]++) {

                // break if all tiles have been considered
                cnt = 4;
                if (cnt > viewport_tile.size())
                  break;

                // break if bandwidth is insuficient
                tmp_sum[3] = 0;
                if (tile_layer[3] > 0) {
                  for (i = 1; i <= tile_layer[3]; i++)
                    tmp_sum[3] += meta.enha_layer_br[i - 1];
                }
                if (tmp_sum[0] + tmp_sum[1] + tmp_sum[2] + tmp_sum[3] +
                        meta.base_layer_br >
                    metric.iter_est_thrp[iter_id] *
                        metric.iter_down_time_budget[iter_id] /
                        (meta.SD * 1000))
                  break;

                // update optimal solution if all tiles have been looped
                if (cnt == viewport_tile.size()) {
                  vp_psnr = 0;
                  for (i = 0; i < cnt; i++) {
                    selected_tile_layer[i] = tile_layer[i];
                    vp_psnr += enha_layer_psnr[tile_layer[i]] *
                               tile_pixel[viewport_tile[i] - 1] / VIEWPORT_SIZE;
                    if (vp_psnr > max_vp_psnr) {
                      max_vp_psnr = vp_psnr;
                    }
                  }
                }

                // 5th tile
                for (tile_layer[4] = 0; tile_layer[4] < 3; tile_layer[4]++) {

                  // break if all tiles have been considered
                  cnt = 5;
                  if (cnt > viewport_tile.size())
                    break;

                  // break if bandwidth is insuficient
                  tmp_sum[4] = 0;
                  if (tile_layer[4] > 0) {
                    for (i = 1; i <= tile_layer[4]; i++)
                      tmp_sum[4] += meta.enha_layer_br[i - 1];
                  }
                  if (tmp_sum[0] + tmp_sum[1] + tmp_sum[2] + tmp_sum[3] +
                          tmp_sum[4] + meta.base_layer_br >
                      metric.iter_est_thrp[iter_id] *
                          metric.iter_down_time_budget[iter_id] /
                          (meta.SD * 1000))
                    break;

                  // update optimal solution if all tiles have been looped
                  if (cnt == viewport_tile.size()) {
                    vp_psnr = 0;
                    for (i = 0; i < cnt; i++) {
                      selected_tile_layer[i] = tile_layer[i];
                      vp_psnr += enha_layer_psnr[tile_layer[i]] *
                                 tile_pixel[viewport_tile[i] - 1] /
                                 VIEWPORT_SIZE;
                      if (vp_psnr > max_vp_psnr) {
                        max_vp_psnr = vp_psnr;
                      }
                    }
                  }

                  // 6th tile
                  for (tile_layer[5] = 0; tile_layer[5] < 3; tile_layer[5]++) {

                    // break if all tiles have been considered
                    cnt = 6;
                    if (cnt > viewport_tile.size())
                      break;

                    // break if bandwidth is insuficient
                    tmp_sum[5] = 0;
                    if (tile_layer[5] > 0) {
                      for (i = 1; i <= tile_layer[5]; i++)
                        tmp_sum[5] += meta.enha_layer_br[i - 1];
                    }
                    if (tmp_sum[0] + tmp_sum[1] + tmp_sum[2] + tmp_sum[3] +
                            tmp_sum[4] + tmp_sum[5] + meta.base_layer_br >
                        metric.iter_est_thrp[iter_id] *
                            metric.iter_down_time_budget[iter_id] /
                            (meta.SD * 1000))
                      break;

                    // update optimal solution if all tiles have been looped
                    if (cnt == viewport_tile.size()) {
                      vp_psnr = 0;
                      for (i = 0; i < cnt; i++) {
                        selected_tile_layer[i] = tile_layer[i];
                        vp_psnr += enha_layer_psnr[tile_layer[i]] *
                                   tile_pixel[viewport_tile[i] - 1] /
                                   VIEWPORT_SIZE;
                        if (vp_psnr > max_vp_psnr) {
                          max_vp_psnr = vp_psnr;
                        }
                      }
                    }

                    // 7th tile
                    for (tile_layer[6] = 0; tile_layer[6] < 3;
                         tile_layer[6]++) {

                      // break if all tiles have been considered
                      cnt = 7;
                      if (cnt > viewport_tile.size())
                        break;

                      // break if bandwidth is insuficient
                      tmp_sum[6] = 0;
                      if (tile_layer[6] > 0) {
                        for (i = 1; i <= tile_layer[6]; i++)
                          tmp_sum[6] += meta.enha_layer_br[i - 1];
                      }
                      if (tmp_sum[0] + tmp_sum[1] + tmp_sum[2] + tmp_sum[3] +
                              tmp_sum[4] + tmp_sum[5] + tmp_sum[6] +
                              meta.base_layer_br >
                          metric.iter_est_thrp[iter_id] *
                              metric.iter_down_time_budget[iter_id] /
                              (meta.SD * 1000))
                        break;

                      // update optimal solution if all tiles have been looped
                      if (cnt == viewport_tile.size()) {
                        vp_psnr = 0;
                        for (i = 0; i < cnt; i++) {
                          selected_tile_layer[i] = tile_layer[i];
                          vp_psnr += enha_layer_psnr[tile_layer[i]] *
                                     tile_pixel[viewport_tile[i] - 1] /
                                     VIEWPORT_SIZE;
                          if (vp_psnr > max_vp_psnr) {
                            max_vp_psnr = vp_psnr;
                          }
                        }
                      }

                      // 8th tile
                      for (tile_layer[7] = 0; tile_layer[7] < 3;
                           tile_layer[7]++) {

                        // break if all tiles have been considered
                        cnt = 8;
                        if (cnt > viewport_tile.size())
                          break;

                        // break if bandwidth is insuficient
                        tmp_sum[7] = 0;
                        if (tile_layer[7] > 0) {
                          for (i = 1; i <= tile_layer[7]; i++)
                            tmp_sum[7] += meta.enha_layer_br[i - 1];
                        }
                        if (tmp_sum[0] + tmp_sum[1] + tmp_sum[2] + tmp_sum[3] +
                                tmp_sum[4] + tmp_sum[5] + tmp_sum[6] +
                                tmp_sum[7] + meta.base_layer_br >
                            metric.iter_est_thrp[iter_id] *
                                metric.iter_down_time_budget[iter_id] /
                                (meta.SD * 1000))
                          break;

                        // update optimal solution if all tiles have been looped
                        if (cnt == viewport_tile.size()) {
                          vp_psnr = 0;
                          for (i = 0; i < cnt; i++) {
                            selected_tile_layer[i] = tile_layer[i];
                            vp_psnr += enha_layer_psnr[tile_layer[i]] *
                                       tile_pixel[viewport_tile[i] - 1] /
                                       VIEWPORT_SIZE;
                            if (vp_psnr > max_vp_psnr) {
                              max_vp_psnr = vp_psnr;
                            }
                          }
                        }

                        // 9th tile
                        for (tile_layer[8] = 0; tile_layer[8] < 3;
                             tile_layer[8]++) {

                          // break if all tiles have been considered
                          cnt = 9;
                          if (cnt > viewport_tile.size())
                            break;

                          // break if bandwidth is insuficient
                          tmp_sum[8] = 0;
                          if (tile_layer[8] > 0) {
                            for (i = 1; i <= tile_layer[8]; i++)
                              tmp_sum[8] += meta.enha_layer_br[i - 1];
                          }
                          if (tmp_sum[0] + tmp_sum[1] + tmp_sum[2] +
                                  tmp_sum[3] + tmp_sum[4] + tmp_sum[5] +
                                  tmp_sum[6] + tmp_sum[7] + tmp_sum[8] +
                                  meta.base_layer_br >
                              metric.iter_est_thrp[iter_id] *
                                  metric.iter_down_time_budget[iter_id] /
                                  (meta.SD * 1000))
                            break;

                          // update optimal solution if all tiles have been
                          // looped
                          if (cnt == viewport_tile.size()) {
                            vp_psnr = 0;
                            for (i = 0; i < cnt; i++) {
                              selected_tile_layer[i] = tile_layer[i];
                              vp_psnr += enha_layer_psnr[tile_layer[i]] *
                                         tile_pixel[viewport_tile[i] - 1] /
                                         VIEWPORT_SIZE;
                              if (vp_psnr > max_vp_psnr) {
                                max_vp_psnr = vp_psnr;
                              }
                            }
                          }

                          // 10th tile
                          for (tile_layer[9] = 0; tile_layer[9] < 3;
                               tile_layer[9]++) {

                            // break if all tiles have been considered
                            cnt = 10;
                            if (cnt > viewport_tile.size())
                              break;

                            // break if bandwidth is insuficient
                            tmp_sum[9] = 0;
                            if (tile_layer[9] > 0) {
                              for (i = 1; i <= tile_layer[9]; i++)
                                tmp_sum[9] += meta.enha_layer_br[i - 1];
                            }
                            if (tmp_sum[0] + tmp_sum[1] + tmp_sum[2] +
                                    tmp_sum[3] + tmp_sum[4] + tmp_sum[5] +
                                    tmp_sum[6] + tmp_sum[7] + tmp_sum[8] +
                                    tmp_sum[9] + meta.base_layer_br >
                                metric.iter_est_thrp[iter_id] *
                                    metric.iter_down_time_budget[iter_id] /
                                    (meta.SD * 1000))
                              break;

                            // update optimal solution if all tiles have been
                            // looped
                            if (cnt == viewport_tile.size()) {
                              vp_psnr = 0;
                              for (i = 0; i < cnt; i++) {
                                selected_tile_layer[i] = tile_layer[i];
                                vp_psnr += enha_layer_psnr[tile_layer[i]] *
                                           tile_pixel[viewport_tile[i] - 1] /
                                           VIEWPORT_SIZE;
                                if (vp_psnr > max_vp_psnr) {
                                  max_vp_psnr = vp_psnr;
                                }
                              }
                            }
                          }
                        }
                      }
                    }
                  }
                }
              }
            }
          }
        }
        auto end = chrono::steady_clock::now();
        // request tiles' layers
        cout << "########### ALG4" << endl;
        cout << "Calc. Time: "
             << chrono::duration_cast<chrono::milliseconds>(end - start).count()
             << endl;
        cout << "budget: " << metric.iter_down_time_budget[iter_id] << ","
             << meta.SD << endl;
        for (i = 0; i < viewport_tile.size(); i++) {
          cout << "(" << viewport_tile[i] << "," << selected_tile_layer[i]
               << "),";
        }
        cout << endl;
        for (layerId = 1; layerId <= 2; layerId++) {
          for (i = 0; i < viewport_tile.size(); i++) {
            if (selected_tile_layer[i] >= layerId) {
              /* assign bitrate and weights */
              metric.face_br[EL_seg_id][viewport_tile[i]][layerId] =
                  meta.enha_layer_br[layerId - 1];
              cout << meta.enha_layer_br[layerId - 1] << endl;

              metric.face_ini_weight[EL_seg_id][viewport_tile[i]][layerId] = 20;

              /* send request */
              metric.face_stream_ID[EL_seg_id][viewport_tile[i]]
                                   [layerId] = send_get_request(
                  client, EL_seg_id, viewport_tile[i],
                  metric.face_br[EL_seg_id][viewport_tile[i]][layerId], 0,
                  metric.face_ini_weight[EL_seg_id][viewport_tile[i]][layerId]);
              cout
                  << "#[UTD2017_adaptation_vr] req_stream_id: "
                  << metric.face_stream_ID[EL_seg_id][viewport_tile[i]][layerId]
                  << "," << metric.face_br[EL_seg_id][viewport_tile[i]][layerId]
                  << endl;
              metric.iter_req_stream[iter_id].push_back(
                  metric.face_stream_ID[EL_seg_id][viewport_tile[i]][layerId]);

              /* map stream_id -> (index, faceid) */
              streamInfo = new StreamInfo(EL_seg_id, viewport_tile[i], layerId);
              metric.RequestStreamInfo.insert(std::pair<int, StreamInfo *>(
                  metric.face_stream_ID[EL_seg_id][viewport_tile[i]][layerId],
                  streamInfo));

              /* record request tiles */
              metric.iter_tile_list[iter_id].push_back(viewport_tile[i]);

              //
              req_s_num++;
            }
          }
        }
        fin_s_num = 0;
      }

      if (meta.ALG == 3 || meta.ALG == 5 || meta.ALG == 6 || meta.ALG == 7 ||
          meta.ALG == 8 || meta.ALG == 9 || meta.ALG == 10 || meta.ALG == 11 ||
          meta.ALG == 12 || meta.ALG == 13) {
        // get number of pixels in viewport of each tile
        // Number of tile: meta.NO_TILE
        // List of tiles in viewport: viewport_tile
        pri = viewport_tile;
        tile_pixel_2 = player.vmask->get_pixel(cur_vp_pos);
        int h, t;
        double last_diff = 0;
        int max_adj_layer = 0;
        vector<int> adj_viewport_tile;
        int dep_stream_id = 0;
        double alpha = 0.9;
        double thrp_budget;
        cout << pri.size() << endl;
        cout << "pri = ";
        for (h = 0; h < pri.size(); h++) {
          cout << pri[h] << ", ";
        }
        cout << endl;
        cout << "tile pixel = ";
        for (t = 0; t < 25; t++) {
          cout << tile_pixel_2[t] << ", ";
        }
        cout << endl;

        metric.iter_down_time_budget[iter_id] =
            player.seg_playback_deadline[EL_seg_id] -
            (std::chrono::duration_cast<std::chrono::microseconds>(
                 std::chrono::steady_clock::now() -
                 client->timing.connect_end_time)
                 .count() /
             1000.0);
        cout << "time_budget: " << metric.iter_down_time_budget[iter_id]
             << endl;
        // Duc
        // Bubble sort viewport tile according to number of pixels
        bool swapped = false;
        int tmp;
        do {
          swapped = false;
          for (j = 0; j < pri.size() - 1; j++) {
            if (tile_pixel_2[pri[j] - 1] < tile_pixel_2[pri[j + 1] - 1]) {
              tmp = pri[j];
              pri[j] = pri[j + 1];
              pri[j + 1] = tmp;
              swapped = true;
            }
          }
        } while (swapped);

        cout << "pri = ";
        for (h = 0; h < pri.size(); h++) {
          cout << pri[h] << ", ";
          /* record request tiles */
          metric.iter_tile_list[iter_id].push_back(pri[h]);
        }
        cout << endl;

        /* calculate remaing time to download */
        metric.iter_down_time_budget[iter_id] =
            player.seg_playback_deadline[EL_seg_id] -
            (std::chrono::duration_cast<std::chrono::microseconds>(
                 std::chrono::steady_clock::now() -
                 client->timing.connect_end_time)
                 .count() /
             1000.0);
        cout << "time_budget: " << metric.iter_down_time_budget[iter_id]
             << endl;
        /* record selected tiles' versions */
        // metric.iter_tile_version[iter_id] = l;

        /* set maximum number of concurrent streams */
        nghttp2_settings_entry iv;
        iv.settings_id = NGHTTP2_SETTINGS_MAX_CONCURRENT_STREAMS;
        iv.value = 1;
        int niv = 1;
        int rv = nghttp2_submit_settings(client->session, NGHTTP2_FLAG_NONE,
                                         &iv, niv);
        if (rv != 0) {
          exit(-1);
        }

        sum_tile_br = meta.base_layer_br;

        //
        if (meta.ALG == 8 || meta.ALG == 11) {
          thrp_budget = metric.iter_smooth_bitrate[iter_id];
        } else {
          thrp_budget = metric.iter_est_thrp[iter_id];
        }

        // extend viewport quality
        if (meta.ALG == 6 || meta.ALG == 7 || meta.ALG == 8 || meta.ALG == 10 ||
            meta.ALG == 11 || meta.ALG == 12 || meta.ALG == 13) {
          // calculate average diff of last segment
          for (i = 1; i <= meta.FPS * 3; i++) {
            last_diff +=
                player.frame_vp_err[player.playing_seg_id * meta.FPS - i];
          }
          last_diff = last_diff / (meta.FPS * 3);
          cout << "last_diff=" << last_diff << endl;

          if (meta.ALG == 6 || meta.ALG == 8 || meta.ALG == 11) {
            // request adjacent tiles
            if (last_diff > 25) {
              max_adj_layer = 1;
              adj_viewport_tile =
                  player.vmask->get_adjacent_tile(cur_vp_pos, 3);
            } else if (last_diff <= 25 && last_diff > 12) {
              max_adj_layer = 1;
              adj_viewport_tile =
                  player.vmask->get_adjacent_tile(cur_vp_pos, 2);
            } else if (last_diff <= 12 && last_diff > 5) {
              max_adj_layer = 1;
              adj_viewport_tile =
                  player.vmask->get_adjacent_tile(cur_vp_pos, 1);
            } else {
              max_adj_layer = 0;
            }

            if (max_adj_layer > 0) {
              cout << "diff = " << last_diff << endl;
              for (layerId = 1; layerId <= max_adj_layer; layerId++) {
                ID = 0;
                while (ID < adj_viewport_tile.size() &&
                       sum_tile_br + meta.enha_layer_br[layerId - 1] <
                           alpha * thrp_budget) {
                  /* assign bitrate and weights */
                  metric.face_br[EL_seg_id][adj_viewport_tile[ID]][layerId] =
                      meta.enha_layer_br[layerId - 1];
                  cout << meta.enha_layer_br[layerId - 1] << endl;

                  metric.face_ini_weight[EL_seg_id][adj_viewport_tile[ID]]
                                        [layerId] = 20;

                  /* send request */
                  metric.face_stream_ID[EL_seg_id][adj_viewport_tile[ID]]
                                       [layerId] = send_get_request(
                      client, EL_seg_id, adj_viewport_tile[ID],
                      metric.face_br[EL_seg_id][adj_viewport_tile[ID]][layerId],
                      dep_stream_id,
                      metric.face_ini_weight[EL_seg_id][adj_viewport_tile[ID]]
                                            [layerId]);
                  dep_stream_id =
                      metric.face_stream_ID[EL_seg_id][adj_viewport_tile[ID]]
                                           [layerId];

                  cout << "###########[adj_viewport_tile] req_stream_id: "
                       << metric.face_stream_ID[EL_seg_id]
                                               [adj_viewport_tile[ID]][layerId]
                       << "," << adj_viewport_tile[ID] << ","
                       << metric.face_br[EL_seg_id][adj_viewport_tile[ID]]
                                        [layerId]
                       << endl;
                  metric.iter_req_stream[iter_id].push_back(
                      metric.face_stream_ID[EL_seg_id][adj_viewport_tile[ID]]
                                           [layerId]);

                  /* map stream_id -> (index, faceid) */
                  streamInfo =
                      new StreamInfo(EL_seg_id, adj_viewport_tile[ID], layerId);
                  metric.RequestStreamInfo.insert(std::pair<int, StreamInfo *>(
                      metric.face_stream_ID[EL_seg_id][adj_viewport_tile[ID]]
                                           [layerId],
                      streamInfo));

                  //
                  req_s_num++;
                  sum_tile_br += meta.enha_layer_br[layerId - 1];
                  ID += 1;
                }
              }
            }
          }
          // ALG-7
          if (meta.ALG == 7) {
            cout << "#########ALG7: " << player.playing_seg_id + 1 << ","
                 << last_diff << endl;
            last_diff = 0;
            for (i = 1; i <= meta.FPS; i++) {
              last_diff +=
                  player.frame_vp_err[player.playing_seg_id * meta.FPS - i];
            }
            last_diff = last_diff / (meta.FPS);

            // idea 1: error of last segment
            // if(player.playing_seg_id > 3 && last_diff > 20){
            // 	adj_viewport_tile = player.vmask->get_adjacent_tile(cur_vp_pos,
            // 1);
            // }

            // idea 2: errors of last two segments
            if (player.playing_seg_id > 6 &&
                (player.seg_vp_err[player.playing_seg_id - 1] > 20 ||
                 player.seg_vp_err[player.playing_seg_id - 2] > 20)) {
              adj_viewport_tile =
                  player.vmask->get_adjacent_tile(cur_vp_pos, 1);
              player.ext_width[player.playing_seg_id] = 1;
            }

            vector<int> tile_set = pri;
            tile_set.insert(tile_set.end(), adj_viewport_tile.begin(),
                            adj_viewport_tile.end());
            cout << "#########ALG7: " << tile_set.size() << endl;
            for (layerId = 1; layerId <= meta.EL_VER_NUM; layerId++) {
              ID = 0;
              while (ID < tile_set.size() &&
                     sum_tile_br + meta.enha_layer_br[layerId - 1] <
                         alpha * thrp_budget) {
                /* assign bitrate and weights */
                metric.face_br[EL_seg_id][tile_set[ID]][layerId] =
                    meta.enha_layer_br[layerId - 1];
                cout << meta.enha_layer_br[layerId - 1] << endl;

                metric.face_ini_weight[EL_seg_id][tile_set[ID]][layerId] = 20;

                /* send request */
                metric.face_stream_ID[EL_seg_id][tile_set[ID]]
                                     [layerId] = send_get_request(
                    client, EL_seg_id, tile_set[ID],
                    metric.face_br[EL_seg_id][tile_set[ID]][layerId],
                    dep_stream_id,
                    metric.face_ini_weight[EL_seg_id][tile_set[ID]][layerId]);
                dep_stream_id =
                    metric.face_stream_ID[EL_seg_id][tile_set[ID]][layerId];

                cout << "###########[ALG 3] req_stream_id: "
                     << metric.face_stream_ID[EL_seg_id][tile_set[ID]][layerId]
                     << "," << tile_set[ID] << ","
                     << metric.face_br[EL_seg_id][tile_set[ID]][layerId]
                     << endl;
                metric.iter_req_stream[iter_id].push_back(
                    metric.face_stream_ID[EL_seg_id][tile_set[ID]][layerId]);

                /* map stream_id -> (index, faceid) */
                streamInfo = new StreamInfo(EL_seg_id, tile_set[ID], layerId);
                metric.RequestStreamInfo.insert(std::pair<int, StreamInfo *>(
                    metric.face_stream_ID[EL_seg_id][tile_set[ID]][layerId],
                    streamInfo));

                //
                req_s_num++;
                sum_tile_br += meta.enha_layer_br[layerId - 1];
                ID += 1;
              }
            }
          }
          // ALG-10
          if (meta.ALG == 10) {
            cout << "#########ALG10: " << player.playing_seg_id + 1 << ","
                 << last_diff << endl;
            last_diff = 0;
            for (i = 1; i <= meta.FPS; i++) {
              last_diff +=
                  player.frame_vp_err[player.playing_seg_id * meta.FPS - i];
            }
            last_diff = last_diff / (meta.FPS);

            // idea 1: error of last segment
            // if(player.playing_seg_id > 3 && last_diff > 20){
            // 	adj_viewport_tile = player.vmask->get_adjacent_tile(cur_vp_pos,
            // 1);
            // }

            // idea 2: errors of last two segments
            if (player.playing_seg_id > 6 &&
                (player.seg_vp_err[player.playing_seg_id - 1] > 20 ||
                 player.seg_vp_err[player.playing_seg_id - 2] > 20)) {
              adj_viewport_tile =
                  player.vmask->get_adjacent_tile(cur_vp_pos, 1);
            }

            if (max_layerId > meta.EL_VER_NUM)
              max_layerId = meta.EL_VER_NUM;
            if (min_layerId < 1)
              min_layerId = 1;
            cout << "max layer ID = " << max_layerId
                 << "; min layer ID = " << min_layerId << endl;
            max_temp = min_layerId;
            min_temp = max_layerId;
            //
            vector<int> tile_set = pri;
            tile_set.insert(tile_set.end(), adj_viewport_tile.begin(),
                            adj_viewport_tile.end());
            cout << "#########ALG7: " << tile_set.size() << endl;
            for (layerId = min_layerId; layerId <= max_layerId; layerId++) {
              ID = 0;
              while (ID < tile_set.size() &&
                     sum_tile_br + meta.enha_layer_br[layerId - 1] <
                         alpha * thrp_budget) {
                /* assign bitrate and weights */
                metric.face_br[EL_seg_id][tile_set[ID]][layerId] =
                    meta.enha_layer_br[layerId - 1];
                cout << meta.enha_layer_br[layerId - 1] << endl;

                metric.face_ini_weight[EL_seg_id][tile_set[ID]][layerId] = 20;

                /* send request */
                metric.face_stream_ID[EL_seg_id][tile_set[ID]]
                                     [layerId] = send_get_request(
                    client, EL_seg_id, tile_set[ID],
                    metric.face_br[EL_seg_id][tile_set[ID]][layerId],
                    dep_stream_id,
                    metric.face_ini_weight[EL_seg_id][tile_set[ID]][layerId]);
                dep_stream_id =
                    metric.face_stream_ID[EL_seg_id][tile_set[ID]][layerId];

                cout << "###########[ALG 3] req_stream_id: "
                     << metric.face_stream_ID[EL_seg_id][tile_set[ID]][layerId]
                     << "," << tile_set[ID] << ","
                     << metric.face_br[EL_seg_id][tile_set[ID]][layerId]
                     << endl;
                metric.iter_req_stream[iter_id].push_back(
                    metric.face_stream_ID[EL_seg_id][tile_set[ID]][layerId]);

                /* map stream_id -> (index, faceid) */
                streamInfo = new StreamInfo(EL_seg_id, tile_set[ID], layerId);
                metric.RequestStreamInfo.insert(std::pair<int, StreamInfo *>(
                    metric.face_stream_ID[EL_seg_id][tile_set[ID]][layerId],
                    streamInfo));

                //
                req_s_num++;
                sum_tile_br += meta.enha_layer_br[layerId - 1];
                ID += 1;
                if (layerId > max_temp)
                  max_temp = layerId;
                if (layerId < min_temp)
                  min_temp = layerId;
                cout << "max temp = " << max_temp << "; min temp = " << min_temp
                     << endl;
              }
            }
            max_layerId = max_temp + 1;
            min_layerId = min_temp - 1;
          }
          // ALG-7
          if (meta.ALG == 12) {
            cout << "#########ALG12: " << player.playing_seg_id + 1 << ","
                 << last_diff << endl;
            last_diff = 0;
            for (i = 1; i <= meta.FPS; i++) {
              last_diff +=
                  player.frame_vp_err[player.playing_seg_id * meta.FPS - i];
            }
            last_diff = last_diff / (meta.FPS);

            // idea 1: error of last segment
            // if(player.playing_seg_id > 3 && last_diff > 20){
            // 	adj_viewport_tile = player.vmask->get_adjacent_tile(cur_vp_pos,
            // 1);
            // }

            // idea 2: errors of last two segments
            // if(player.playing_seg_id > 6 &&
            // (player.seg_vp_err[player.playing_seg_id-1] > 20 ||
            // player.seg_vp_err[player.playing_seg_id-2] > 20)){
            // 	adj_viewport_tile = player.vmask->get_adjacent_tile(cur_vp_pos,
            // 1);
            // }

            // idea 3:
            if (player.playing_seg_id > 6 &&
                player.seg_smooth_vp_err[player.playing_seg_id - 1] > 20) {
              player.ext_width[player.playing_seg_id] = 1;
              adj_viewport_tile =
                  player.vmask->get_adjacent_tile(cur_vp_pos, 1);
            } else {
              player.ext_width[player.playing_seg_id] = 0;
            }

            // idea 4:
            // if(player.playing_seg_id > 6){
            // 	if(player.seg_smooth_vp_err[player.playing_seg_id-1] > 40){
            // 		player.ext_width[player.playing_seg_id] = 2;
            // 		adj_viewport_tile =
            // player.vmask->get_adjacent_tile(cur_vp_pos, 2); 	}else{
            // if(player.seg_smooth_vp_err[player.playing_seg_id-1] > 20){
            // player.ext_width[player.playing_seg_id] = 1;
            // adj_viewport_tile = player.vmask->get_adjacent_tile(cur_vp_pos,
            // 1);
            // }else{ 			player.ext_width[player.playing_seg_id]
            // = 0;
            // 		}
            // 	}
            // }

            vector<int> tile_set = pri;
            tile_set.insert(tile_set.end(), adj_viewport_tile.begin(),
                            adj_viewport_tile.end());
            cout << "#########ALG7: " << tile_set.size() << endl;
            for (layerId = 1; layerId <= meta.EL_VER_NUM; layerId++) {
              ID = 0;
              while (ID < tile_set.size() &&
                     sum_tile_br + meta.enha_layer_br[layerId - 1] <
                         alpha * thrp_budget) {
                /* assign bitrate and weights */
                metric.face_br[EL_seg_id][tile_set[ID]][layerId] =
                    meta.enha_layer_br[layerId - 1];
                cout << meta.enha_layer_br[layerId - 1] << endl;

                metric.face_ini_weight[EL_seg_id][tile_set[ID]][layerId] = 20;

                /* send request */
                metric.face_stream_ID[EL_seg_id][tile_set[ID]]
                                     [layerId] = send_get_request(
                    client, EL_seg_id, tile_set[ID],
                    metric.face_br[EL_seg_id][tile_set[ID]][layerId],
                    dep_stream_id,
                    metric.face_ini_weight[EL_seg_id][tile_set[ID]][layerId]);
                dep_stream_id =
                    metric.face_stream_ID[EL_seg_id][tile_set[ID]][layerId];

                cout << "###########[ALG 3] req_stream_id: "
                     << metric.face_stream_ID[EL_seg_id][tile_set[ID]][layerId]
                     << "," << tile_set[ID] << ","
                     << metric.face_br[EL_seg_id][tile_set[ID]][layerId]
                     << endl;
                metric.iter_req_stream[iter_id].push_back(
                    metric.face_stream_ID[EL_seg_id][tile_set[ID]][layerId]);

                /* map stream_id -> (index, faceid) */
                streamInfo = new StreamInfo(EL_seg_id, tile_set[ID], layerId);
                metric.RequestStreamInfo.insert(std::pair<int, StreamInfo *>(
                    metric.face_stream_ID[EL_seg_id][tile_set[ID]][layerId],
                    streamInfo));

                //
                req_s_num++;
                sum_tile_br += meta.enha_layer_br[layerId - 1];
                ID += 1;
              }
            }
          }

          if (meta.ALG == 13) {
            cout << "#########ALG13: " << player.playing_seg_id + 1 << ","
                 << last_diff << endl;
            last_diff = 0;
            for (i = 1; i <= meta.FPS; i++) {
              last_diff +=
                  player.frame_vp_err[player.playing_seg_id * meta.FPS - i];
            }
            last_diff = last_diff / (meta.FPS);

            // idea 3:
            if (player.playing_seg_id > 6 &&
                // player.seg_vp_err_tile[player.playing_seg_id - 1] > 4 &&
                // player.seg_smooth_vp_err[player.playing_seg_id - 7] > 20 &&
                // player.seg_smooth_vp_err[player.playing_seg_id - 6] > 20 &&
                player.seg_smooth_vp_err[player.playing_seg_id - 5] > 20 &&
                player.seg_smooth_vp_err[player.playing_seg_id - 4] > 20 &&
                player.seg_smooth_vp_err[player.playing_seg_id - 3] > 20 &&
                player.seg_smooth_vp_err[player.playing_seg_id - 2] > 20 &&
                player.seg_smooth_vp_err[player.playing_seg_id - 1] > 20) {
              player.ext_width[player.playing_seg_id] = 1;
              adj_viewport_tile =
                  player.vmask->get_adjacent_tile(cur_vp_pos, 1);
              min_layerId = 1;
              max_layerId = meta.EL_VER_NUM;
              cout << "[IF]: max layer ID = " << max_layerId
                   << "; min layer ID = " << min_layerId << endl;
            } else {
              player.ext_width[player.playing_seg_id] = 0;
              if (max_layerId > meta.EL_VER_NUM)
                max_layerId = meta.EL_VER_NUM;
              if (min_layerId < 1)
                min_layerId = 1;
              cout << "[ELSE]: max layer ID = " << max_layerId
                   << "; min layer ID = " << min_layerId << endl;
              max_temp = min_layerId;
              min_temp = max_layerId;
            }

            vector<int> tile_set = pri;
            tile_set.insert(tile_set.end(), adj_viewport_tile.begin(),
                            adj_viewport_tile.end());
            cout << "#########ALG13: " << tile_set.size() << endl;
            for (layerId = min_layerId; layerId <= max_layerId; layerId++) {
              ID = 0;
              while (ID < tile_set.size() &&
                     sum_tile_br + meta.enha_layer_br[layerId - 1] <
                         alpha * thrp_budget) {
                /* assign bitrate and weights */
                metric.face_br[EL_seg_id][tile_set[ID]][layerId] =
                    meta.enha_layer_br[layerId - 1];
                cout << meta.enha_layer_br[layerId - 1] << endl;

                metric.face_ini_weight[EL_seg_id][tile_set[ID]][layerId] = 20;

                /* send request */
                metric.face_stream_ID[EL_seg_id][tile_set[ID]]
                                     [layerId] = send_get_request(
                    client, EL_seg_id, tile_set[ID],
                    metric.face_br[EL_seg_id][tile_set[ID]][layerId],
                    dep_stream_id,
                    metric.face_ini_weight[EL_seg_id][tile_set[ID]][layerId]);
                dep_stream_id =
                    metric.face_stream_ID[EL_seg_id][tile_set[ID]][layerId];

                cout << "###########[ALG 13] req_stream_id: "
                     << metric.face_stream_ID[EL_seg_id][tile_set[ID]][layerId]
                     << "," << tile_set[ID] << ","
                     << metric.face_br[EL_seg_id][tile_set[ID]][layerId]
                     << endl;
                metric.iter_req_stream[iter_id].push_back(
                    metric.face_stream_ID[EL_seg_id][tile_set[ID]][layerId]);

                /* map stream_id -> (index, faceid) */
                streamInfo = new StreamInfo(EL_seg_id, tile_set[ID], layerId);
                metric.RequestStreamInfo.insert(std::pair<int, StreamInfo *>(
                    metric.face_stream_ID[EL_seg_id][tile_set[ID]][layerId],
                    streamInfo));

                //
                req_s_num++;
                sum_tile_br += meta.enha_layer_br[layerId - 1];
                ID += 1;
                if (layerId > max_temp)
                  max_temp = layerId;
                if (layerId < min_temp)
                  min_temp = layerId;
                cout << "max temp = " << max_temp << "; min temp = " << min_temp
                     << endl;
              }
            }
            max_layerId = max_temp + 1;
            min_layerId = min_temp - 1;
          }
        }
        if (meta.ALG == 5 || meta.ALG == 6 || meta.ALG == 8 || meta.ALG == 11) {
          cout << "ID = " << ID << endl;
          for (layerId = 1; layerId <= meta.EL_VER_NUM; layerId++) {
            ID = 0;
            while (ID < pri.size() &&
                   sum_tile_br + meta.enha_layer_br[layerId - 1] <
                       alpha * thrp_budget) {

              /* assign bitrate and weights */
              metric.face_br[EL_seg_id][pri[ID]][layerId] =
                  meta.enha_layer_br[layerId - 1];
              cout << meta.enha_layer_br[layerId - 1] << endl;

              metric.face_ini_weight[EL_seg_id][pri[ID]][layerId] = 20;

              /* send request */
              metric.face_stream_ID[EL_seg_id][pri[ID]][layerId] =
                  send_get_request(
                      client, EL_seg_id, pri[ID],
                      metric.face_br[EL_seg_id][pri[ID]][layerId],
                      dep_stream_id,
                      metric.face_ini_weight[EL_seg_id][pri[ID]][layerId]);
              dep_stream_id =
                  metric.face_stream_ID[EL_seg_id][pri[ID]][layerId];

              cout << "###########[ALG 3] req_stream_id: "
                   << metric.face_stream_ID[EL_seg_id][pri[ID]][layerId] << ","
                   << pri[ID] << ","
                   << metric.face_br[EL_seg_id][pri[ID]][layerId] << endl;
              metric.iter_req_stream[iter_id].push_back(
                  metric.face_stream_ID[EL_seg_id][pri[ID]][layerId]);

              /* map stream_id -> (index, faceid) */
              streamInfo = new StreamInfo(EL_seg_id, pri[ID], layerId);
              metric.RequestStreamInfo.insert(std::pair<int, StreamInfo *>(
                  metric.face_stream_ID[EL_seg_id][pri[ID]][layerId],
                  streamInfo));

              //
              req_s_num++;
              sum_tile_br += meta.enha_layer_br[layerId - 1];
              ID += 1;
              // std::this_thread::sleep_for(std::chrono::milliseconds(2));
            }
          }
        }
        //
        if (meta.ALG == 9) {
          if (max_layerId > meta.EL_VER_NUM)
            max_layerId = meta.EL_VER_NUM;
          if (min_layerId < 1)
            min_layerId = 1;
          cout << "max layer ID = " << max_layerId
               << "; min layer ID = " << min_layerId << endl;
          max_temp = min_layerId;
          min_temp = max_layerId;
          for (layerId = min_layerId; layerId <= max_layerId; layerId++) {
            ID = 0;
            while (ID < pri.size() &&
                   sum_tile_br + meta.enha_layer_br[layerId - 1] <
                       alpha * thrp_budget) {

              /* assign bitrate and weights */
              metric.face_br[EL_seg_id][pri[ID]][layerId] =
                  meta.enha_layer_br[layerId - 1];
              cout << meta.enha_layer_br[layerId - 1] << endl;

              metric.face_ini_weight[EL_seg_id][pri[ID]][layerId] = 20;

              /* send request */
              metric.face_stream_ID[EL_seg_id][pri[ID]][layerId] =
                  send_get_request(
                      client, EL_seg_id, pri[ID],
                      metric.face_br[EL_seg_id][pri[ID]][layerId],
                      dep_stream_id,
                      metric.face_ini_weight[EL_seg_id][pri[ID]][layerId]);
              dep_stream_id =
                  metric.face_stream_ID[EL_seg_id][pri[ID]][layerId];

              cout << "###########[ALG 9] req_stream_id: "
                   << metric.face_stream_ID[EL_seg_id][pri[ID]][layerId] << ","
                   << pri[ID] << ","
                   << metric.face_br[EL_seg_id][pri[ID]][layerId] << endl;
              metric.iter_req_stream[iter_id].push_back(
                  metric.face_stream_ID[EL_seg_id][pri[ID]][layerId]);

              /* map stream_id -> (index, faceid) */
              streamInfo = new StreamInfo(EL_seg_id, pri[ID], layerId);
              metric.RequestStreamInfo.insert(std::pair<int, StreamInfo *>(
                  metric.face_stream_ID[EL_seg_id][pri[ID]][layerId],
                  streamInfo));

              //
              req_s_num++;
              sum_tile_br += meta.enha_layer_br[layerId - 1];
              ID += 1;
              if (layerId > max_temp)
                max_temp = layerId;
              if (layerId < min_temp)
                min_temp = layerId;
              cout << "max temp = " << max_temp << "; min temp = " << min_temp
                   << endl;
              // std::this_thread::sleep_for(std::chrono::milliseconds(2));
            }
          }
          max_layerId = max_temp + 1;
          min_layerId = min_temp - 1;
          // fin_s_num = 0;
        }
        fin_s_num = 0;
      }
      // exit(1);

      if (meta.ALG == 1 || meta.ALG == 2) {

        /* find optimal version for visible tiles */
        for (l = 0; l < meta.EL_VER_NUM; l++) {
          if (meta.base_layer_br +
                  (EL_br_tmp + meta.enha_layer_br[l]) * viewport_tile.size() >
              metric.iter_est_thrp[iter_id])
            break;
          EL_br_tmp += meta.enha_layer_br[l];
        }

        cout << "#[UTD2017]: Enhan. layer: " << l << ","
             << metric.iter_est_thrp[iter_id] << "," << viewport_tile.size()
             << endl;

        /* send request for visible tiles */
        if (l > 0) {
          /* calculate remaing time to download */
          metric.iter_down_time_budget[iter_id] =
              player.seg_playback_deadline[EL_seg_id] -
              (std::chrono::duration_cast<std::chrono::microseconds>(
                   std::chrono::steady_clock::now() -
                   client->timing.connect_end_time)
                   .count() /
               1000.0);

          /* record selected tiles' versions */
          metric.iter_tile_version[iter_id] = l;

          /* set maximum number of concurrent streams */
          nghttp2_settings_entry iv;
          iv.settings_id = NGHTTP2_SETTINGS_MAX_CONCURRENT_STREAMS;
          iv.value = 1;
          int niv = 1;
          int rv = nghttp2_submit_settings(client->session, NGHTTP2_FLAG_NONE,
                                           &iv, niv);
          if (rv != 0) {
            exit(-1);
          }
          int dep_stream_id = 0;
          /* send get requests */
          for (i = 0; i < viewport_tile.size(); i++) {
            for (layerId = 1; layerId <= l; layerId++) {

              /* assign bitrate and weights */
              metric.face_br[EL_seg_id][viewport_tile[i]][layerId] =
                  meta.enha_layer_br[layerId - 1];
              cout << meta.enha_layer_br[layerId - 1] << endl;

              metric.face_ini_weight[EL_seg_id][viewport_tile[i]][layerId] = 20;

              /* send request */
              metric.face_stream_ID[EL_seg_id][viewport_tile[i]]
                                   [layerId] = send_get_request(
                  client, EL_seg_id, viewport_tile[i],
                  metric.face_br[EL_seg_id][viewport_tile[i]][layerId],
                  dep_stream_id,
                  metric.face_ini_weight[EL_seg_id][viewport_tile[i]][layerId]);
              dep_stream_id =
                  metric.face_stream_ID[EL_seg_id][viewport_tile[i]][layerId];

              cout
                  << "#[UTD2017_adaptation_vr] req_stream_id: "
                  << metric.face_stream_ID[EL_seg_id][viewport_tile[i]][layerId]
                  << "," << metric.face_br[EL_seg_id][viewport_tile[i]][layerId]
                  << endl;
              metric.iter_req_stream[iter_id].push_back(
                  metric.face_stream_ID[EL_seg_id][viewport_tile[i]][layerId]);

              /* map stream_id -> (index, faceid) */
              streamInfo = new StreamInfo(EL_seg_id, viewport_tile[i], layerId);
              metric.RequestStreamInfo.insert(std::pair<int, StreamInfo *>(
                  metric.face_stream_ID[EL_seg_id][viewport_tile[i]][layerId],
                  streamInfo));

              /* record request tiles */
              metric.iter_tile_list[iter_id].push_back(viewport_tile[i]);

              //
              req_s_num++;
            }
          }

          /* update status */
          // req_s_num = viewport_tile.size() * l;
          fin_s_num = 0;
        }
      }
    }
  }
  if (req_s_num > 0) {
    /* init status of this round */
    metric.iter_down_data[iter_id] = 0;
    metric.iter_down_data_frame[iter_id] = 0;
    metric.iter_down_time[iter_id] = 0;
  } else {
    /* wait for 100ms, then repeat decision engine */
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    MMSP(client, iter_id);
  }
}
void UTD2017_adaptation_vr(HttpClient *client, int iter_id) {
  int l, i, layerId, j, k, ID;
  int *cur_vp_pos;
  int *tile_pixel;
  int *tile_pixel_2;
  vector<int> pri;
  vector<int> viewport_tile;
  double EL_br_tmp = 0;
  int sum_tile_br;
  StreamInfo *streamInfo;

  cout << "#[UTD2017_adaptation_vr]: iter_id=" << iter_id << ","
       << (player.playing_seg_id + 1) << endl;
  metric.iter_start_time[iter_id] =
      std::chrono::duration_cast<std::chrono::microseconds>(
          get_time() - client->timing.connect_end_time)
          .count() /
      1000.0;

  /* reset cancelled stream */
  cancelled_request_stream.clear();

  /* estimate available bandwidth for this iteration */
  metric.est_iter_thrp(iter_id);
  cout << "#[UTD2017_adaptation_vr]: est_thrp=" << metric.iter_est_thrp[iter_id]
       << endl;
  req_s_num = 0;
  // if(BL_seg_id < meta.NO_SEG - 1){
  if (BL_seg_id < meta.NO_SEG) {
    /* request a BL layer chunk if B_cur < B_thres - 1*/
    // if(player.get_cur_buff_size_second() < meta.BASE_BUFF_THRES - 1){
    if (player.get_cur_buff_size_second() < meta.BASE_BUFF_THRES - 1) {
      /* next base layer segment ID */
      metric.iter_tile_version[iter_id] = 0;
      metric.iter_down_time_budget[iter_id] =
          player.seg_playback_deadline[BL_seg_id] -
          (std::chrono::duration_cast<std::chrono::microseconds>(
               std::chrono::steady_clock::now() -
               client->timing.connect_end_time)
               .count() /
           1000.0);

      /* Download base layer of the current segment */
      metric.face_br[BL_seg_id][0][0] = meta.base_layer_br;
      metric.face_ini_weight[BL_seg_id][0][0] = 20;

      /* send request */
      metric.face_stream_ID[BL_seg_id][0][0] = send_get_request(
          client, BL_seg_id, 0, metric.face_br[BL_seg_id][0][0], 0,
          metric.face_ini_weight[BL_seg_id][0][0]);
      cout << "#[UTD2017_adaptation_vr] req_stream_id: "
           << metric.face_stream_ID[BL_seg_id][0][0] << "," << BL_seg_id
           << endl;

      /* map stream_id -> (index, faceid) */
      streamInfo = new StreamInfo(BL_seg_id, 0, 0);
      metric.RequestStreamInfo.insert(std::pair<int, StreamInfo *>(
          metric.face_stream_ID[BL_seg_id][0][0], streamInfo));
      metric.iter_req_stream[iter_id].push_back(
          metric.face_stream_ID[BL_seg_id][0][0]);

      /* update status */
      req_s_num = 1;
      fin_s_num = 0;
    }
  }
  if (req_s_num == 0) {
    /* decide and request EL layer chunks of tiles of next played segment */
    if (player.playing_seg_id == meta.NO_SEG - 1) {
      return;
    }

    /* headNext(): get next playing segment */
    EL_seg_id = player.playing_seg_id + 1;
    // if(!metric.seg_EL_download[EL_seg_id] && BL_seg_id < meta.NO_SEG){
    if (!metric.seg_EL_download[EL_seg_id] && EL_seg_id < meta.NO_SEG) {

      /* predictViewport */
      cur_vp_pos = player.htrace->get_vp_pos(player.get_cur_play_time());
      viewport_tile = player.vmask->get_visible_tile(cur_vp_pos);

      cout << "#[UTD2017]: " << cur_vp_pos[0] << ", " << cur_vp_pos[1] << ", "
           << player.get_cur_play_time() << endl;

      if (meta.ALG == 4) {
        metric.iter_down_time_budget[iter_id] =
            player.seg_playback_deadline[EL_seg_id] -
            (std::chrono::duration_cast<std::chrono::microseconds>(
                 std::chrono::steady_clock::now() -
                 client->timing.connect_end_time)
                 .count() /
             1000.0);
        cout << "ALG4: " << metric.iter_down_time_budget[iter_id] << endl;
        auto start = chrono::steady_clock::now();
        // layers of tiles(loop variable)
        int *tile_layer = new int[10];

        // selected layers of tiles
        int *selected_tile_layer = new int[viewport_tile.size()];

        // temporal sum at each iteration
        double *tmp_sum = new double[10];
        int cnt = 0;
        double max_vp_psnr = 0, vp_psnr, VIEWPORT_SIZE = 960.0 * 960.0;
        // double enha_layer_psnr[] = {35.0, 37.0, 39.0};
        double enha_layer_psnr[] = {125.0, 325.0, 725.0};

        tile_pixel = player.vmask->get_pixel(cur_vp_pos);

        // nested `for` loops to iterate over all possible combinations

        // 1st viewport tile
        for (tile_layer[0] = 0; tile_layer[0] < 3; tile_layer[0]++) {

          // break if all tiles have been considered
          cnt = 1;
          if (cnt > viewport_tile.size())
            break;

          // break if bandwidth is insuficient
          tmp_sum[0] = 0;
          if (tile_layer[0] > 0) {
            for (i = 1; i <= tile_layer[0]; i++)
              tmp_sum[0] += meta.enha_layer_br[i - 1];
          }
          if (tmp_sum[0] + meta.base_layer_br >
              metric.iter_est_thrp[iter_id] *
                  metric.iter_down_time_budget[iter_id] / (meta.SD * 1000))
            break;

          // update optimal solution if all tiles have been looped
          if (cnt == viewport_tile.size()) {
            vp_psnr = 0;
            for (i = 0; i < cnt; i++) {
              selected_tile_layer[i] = tile_layer[i];
              vp_psnr += enha_layer_psnr[tile_layer[i]] *
                         tile_pixel[viewport_tile[i] - 1] / VIEWPORT_SIZE;
              if (vp_psnr > max_vp_psnr) {
                max_vp_psnr = vp_psnr;
              }
            }
          }

          // 2nd tile
          for (tile_layer[1] = 0; tile_layer[1] < 3; tile_layer[1]++) {

            // break if all tiles have been considered
            cnt = 2;
            if (cnt > viewport_tile.size())
              break;

            // break if bandwidth is insuficient
            tmp_sum[1] = 0;
            if (tile_layer[1] > 0) {
              for (i = 1; i <= tile_layer[1]; i++)
                tmp_sum[1] += meta.enha_layer_br[i - 1];
            }
            if (tmp_sum[0] + tmp_sum[1] + meta.base_layer_br >
                metric.iter_est_thrp[iter_id] *
                    metric.iter_down_time_budget[iter_id] / (meta.SD * 1000))
              break;

            // update optimal solution if all tiles have been looped
            if (cnt == viewport_tile.size()) {
              vp_psnr = 0;
              for (i = 0; i < cnt; i++) {
                selected_tile_layer[i] = tile_layer[i];
                vp_psnr += enha_layer_psnr[tile_layer[i]] *
                           tile_pixel[viewport_tile[i] - 1] / VIEWPORT_SIZE;
                if (vp_psnr > max_vp_psnr) {
                  max_vp_psnr = vp_psnr;
                }
              }
            }

            // 3rd tile
            for (tile_layer[2] = 0; tile_layer[2] < 3; tile_layer[2]++) {

              // break if all tiles have been considered
              cnt = 3;
              if (cnt > viewport_tile.size())
                break;

              // break if bandwidth is insuficient
              tmp_sum[2] = 0;
              if (tile_layer[2] > 0) {
                for (i = 1; i <= tile_layer[2]; i++)
                  tmp_sum[2] += meta.enha_layer_br[i - 1];
              }
              if (tmp_sum[0] + tmp_sum[1] + tmp_sum[2] + meta.base_layer_br >
                  metric.iter_est_thrp[iter_id] *
                      metric.iter_down_time_budget[iter_id] / (meta.SD * 1000))
                break;

              // update optimal solution if all tiles have been looped
              if (cnt == viewport_tile.size()) {
                vp_psnr = 0;
                for (i = 0; i < cnt; i++) {
                  selected_tile_layer[i] = tile_layer[i];
                  vp_psnr += enha_layer_psnr[tile_layer[i]] *
                             tile_pixel[viewport_tile[i] - 1] / VIEWPORT_SIZE;
                  if (vp_psnr > max_vp_psnr) {
                    max_vp_psnr = vp_psnr;
                  }
                }
              }

              // 4th tile
              for (tile_layer[3] = 0; tile_layer[3] < 3; tile_layer[3]++) {

                // break if all tiles have been considered
                cnt = 4;
                if (cnt > viewport_tile.size())
                  break;

                // break if bandwidth is insuficient
                tmp_sum[3] = 0;
                if (tile_layer[3] > 0) {
                  for (i = 1; i <= tile_layer[3]; i++)
                    tmp_sum[3] += meta.enha_layer_br[i - 1];
                }
                if (tmp_sum[0] + tmp_sum[1] + tmp_sum[2] + tmp_sum[3] +
                        meta.base_layer_br >
                    metric.iter_est_thrp[iter_id] *
                        metric.iter_down_time_budget[iter_id] /
                        (meta.SD * 1000))
                  break;

                // update optimal solution if all tiles have been looped
                if (cnt == viewport_tile.size()) {
                  vp_psnr = 0;
                  for (i = 0; i < cnt; i++) {
                    selected_tile_layer[i] = tile_layer[i];
                    vp_psnr += enha_layer_psnr[tile_layer[i]] *
                               tile_pixel[viewport_tile[i] - 1] / VIEWPORT_SIZE;
                    if (vp_psnr > max_vp_psnr) {
                      max_vp_psnr = vp_psnr;
                    }
                  }
                }

                // 5th tile
                for (tile_layer[4] = 0; tile_layer[4] < 3; tile_layer[4]++) {

                  // break if all tiles have been considered
                  cnt = 5;
                  if (cnt > viewport_tile.size())
                    break;

                  // break if bandwidth is insuficient
                  tmp_sum[4] = 0;
                  if (tile_layer[4] > 0) {
                    for (i = 1; i <= tile_layer[4]; i++)
                      tmp_sum[4] += meta.enha_layer_br[i - 1];
                  }
                  if (tmp_sum[0] + tmp_sum[1] + tmp_sum[2] + tmp_sum[3] +
                          tmp_sum[4] + meta.base_layer_br >
                      metric.iter_est_thrp[iter_id] *
                          metric.iter_down_time_budget[iter_id] /
                          (meta.SD * 1000))
                    break;

                  // update optimal solution if all tiles have been looped
                  if (cnt == viewport_tile.size()) {
                    vp_psnr = 0;
                    for (i = 0; i < cnt; i++) {
                      selected_tile_layer[i] = tile_layer[i];
                      vp_psnr += enha_layer_psnr[tile_layer[i]] *
                                 tile_pixel[viewport_tile[i] - 1] /
                                 VIEWPORT_SIZE;
                      if (vp_psnr > max_vp_psnr) {
                        max_vp_psnr = vp_psnr;
                      }
                    }
                  }

                  // 6th tile
                  for (tile_layer[5] = 0; tile_layer[5] < 3; tile_layer[5]++) {

                    // break if all tiles have been considered
                    cnt = 6;
                    if (cnt > viewport_tile.size())
                      break;

                    // break if bandwidth is insuficient
                    tmp_sum[5] = 0;
                    if (tile_layer[5] > 0) {
                      for (i = 1; i <= tile_layer[5]; i++)
                        tmp_sum[5] += meta.enha_layer_br[i - 1];
                    }
                    if (tmp_sum[0] + tmp_sum[1] + tmp_sum[2] + tmp_sum[3] +
                            tmp_sum[4] + tmp_sum[5] + meta.base_layer_br >
                        metric.iter_est_thrp[iter_id] *
                            metric.iter_down_time_budget[iter_id] /
                            (meta.SD * 1000))
                      break;

                    // update optimal solution if all tiles have been looped
                    if (cnt == viewport_tile.size()) {
                      vp_psnr = 0;
                      for (i = 0; i < cnt; i++) {
                        selected_tile_layer[i] = tile_layer[i];
                        vp_psnr += enha_layer_psnr[tile_layer[i]] *
                                   tile_pixel[viewport_tile[i] - 1] /
                                   VIEWPORT_SIZE;
                        if (vp_psnr > max_vp_psnr) {
                          max_vp_psnr = vp_psnr;
                        }
                      }
                    }

                    // 7th tile
                    for (tile_layer[6] = 0; tile_layer[6] < 3;
                         tile_layer[6]++) {

                      // break if all tiles have been considered
                      cnt = 7;
                      if (cnt > viewport_tile.size())
                        break;

                      // break if bandwidth is insuficient
                      tmp_sum[6] = 0;
                      if (tile_layer[6] > 0) {
                        for (i = 1; i <= tile_layer[6]; i++)
                          tmp_sum[6] += meta.enha_layer_br[i - 1];
                      }
                      if (tmp_sum[0] + tmp_sum[1] + tmp_sum[2] + tmp_sum[3] +
                              tmp_sum[4] + tmp_sum[5] + tmp_sum[6] +
                              meta.base_layer_br >
                          metric.iter_est_thrp[iter_id] *
                              metric.iter_down_time_budget[iter_id] /
                              (meta.SD * 1000))
                        break;

                      // update optimal solution if all tiles have been looped
                      if (cnt == viewport_tile.size()) {
                        vp_psnr = 0;
                        for (i = 0; i < cnt; i++) {
                          selected_tile_layer[i] = tile_layer[i];
                          vp_psnr += enha_layer_psnr[tile_layer[i]] *
                                     tile_pixel[viewport_tile[i] - 1] /
                                     VIEWPORT_SIZE;
                          if (vp_psnr > max_vp_psnr) {
                            max_vp_psnr = vp_psnr;
                          }
                        }
                      }

                      // 8th tile
                      for (tile_layer[7] = 0; tile_layer[7] < 3;
                           tile_layer[7]++) {

                        // break if all tiles have been considered
                        cnt = 8;
                        if (cnt > viewport_tile.size())
                          break;

                        // break if bandwidth is insuficient
                        tmp_sum[7] = 0;
                        if (tile_layer[7] > 0) {
                          for (i = 1; i <= tile_layer[7]; i++)
                            tmp_sum[7] += meta.enha_layer_br[i - 1];
                        }
                        if (tmp_sum[0] + tmp_sum[1] + tmp_sum[2] + tmp_sum[3] +
                                tmp_sum[4] + tmp_sum[5] + tmp_sum[6] +
                                tmp_sum[7] + meta.base_layer_br >
                            metric.iter_est_thrp[iter_id] *
                                metric.iter_down_time_budget[iter_id] /
                                (meta.SD * 1000))
                          break;

                        // update optimal solution if all tiles have been looped
                        if (cnt == viewport_tile.size()) {
                          vp_psnr = 0;
                          for (i = 0; i < cnt; i++) {
                            selected_tile_layer[i] = tile_layer[i];
                            vp_psnr += enha_layer_psnr[tile_layer[i]] *
                                       tile_pixel[viewport_tile[i] - 1] /
                                       VIEWPORT_SIZE;
                            if (vp_psnr > max_vp_psnr) {
                              max_vp_psnr = vp_psnr;
                            }
                          }
                        }

                        // 9th tile
                        for (tile_layer[8] = 0; tile_layer[8] < 3;
                             tile_layer[8]++) {

                          // break if all tiles have been considered
                          cnt = 9;
                          if (cnt > viewport_tile.size())
                            break;

                          // break if bandwidth is insuficient
                          tmp_sum[8] = 0;
                          if (tile_layer[8] > 0) {
                            for (i = 1; i <= tile_layer[8]; i++)
                              tmp_sum[8] += meta.enha_layer_br[i - 1];
                          }
                          if (tmp_sum[0] + tmp_sum[1] + tmp_sum[2] +
                                  tmp_sum[3] + tmp_sum[4] + tmp_sum[5] +
                                  tmp_sum[6] + tmp_sum[7] + tmp_sum[8] +
                                  meta.base_layer_br >
                              metric.iter_est_thrp[iter_id] *
                                  metric.iter_down_time_budget[iter_id] /
                                  (meta.SD * 1000))
                            break;

                          // update optimal solution if all tiles have been
                          // looped
                          if (cnt == viewport_tile.size()) {
                            vp_psnr = 0;
                            for (i = 0; i < cnt; i++) {
                              selected_tile_layer[i] = tile_layer[i];
                              vp_psnr += enha_layer_psnr[tile_layer[i]] *
                                         tile_pixel[viewport_tile[i] - 1] /
                                         VIEWPORT_SIZE;
                              if (vp_psnr > max_vp_psnr) {
                                max_vp_psnr = vp_psnr;
                              }
                            }
                          }

                          // 10th tile
                          for (tile_layer[9] = 0; tile_layer[9] < 3;
                               tile_layer[9]++) {

                            // break if all tiles have been considered
                            cnt = 10;
                            if (cnt > viewport_tile.size())
                              break;

                            // break if bandwidth is insuficient
                            tmp_sum[9] = 0;
                            if (tile_layer[9] > 0) {
                              for (i = 1; i <= tile_layer[9]; i++)
                                tmp_sum[9] += meta.enha_layer_br[i - 1];
                            }
                            if (tmp_sum[0] + tmp_sum[1] + tmp_sum[2] +
                                    tmp_sum[3] + tmp_sum[4] + tmp_sum[5] +
                                    tmp_sum[6] + tmp_sum[7] + tmp_sum[8] +
                                    tmp_sum[9] + meta.base_layer_br >
                                metric.iter_est_thrp[iter_id] *
                                    metric.iter_down_time_budget[iter_id] /
                                    (meta.SD * 1000))
                              break;

                            // update optimal solution if all tiles have been
                            // looped
                            if (cnt == viewport_tile.size()) {
                              vp_psnr = 0;
                              for (i = 0; i < cnt; i++) {
                                selected_tile_layer[i] = tile_layer[i];
                                vp_psnr += enha_layer_psnr[tile_layer[i]] *
                                           tile_pixel[viewport_tile[i] - 1] /
                                           VIEWPORT_SIZE;
                                if (vp_psnr > max_vp_psnr) {
                                  max_vp_psnr = vp_psnr;
                                }
                              }
                            }
                          }
                        }
                      }
                    }
                  }
                }
              }
            }
          }
        }
        auto end = chrono::steady_clock::now();
        // request tiles' layers
        cout << "########### ALG4" << endl;
        cout << "Calc. Time: "
             << chrono::duration_cast<chrono::milliseconds>(end - start).count()
             << endl;
        cout << "budget: " << metric.iter_down_time_budget[iter_id] << ","
             << meta.SD << endl;
        for (i = 0; i < viewport_tile.size(); i++) {
          cout << "(" << viewport_tile[i] << "," << selected_tile_layer[i]
               << "),";
        }
        cout << endl;
        for (layerId = 1; layerId <= 2; layerId++) {
          for (i = 0; i < viewport_tile.size(); i++) {
            if (selected_tile_layer[i] >= layerId) {
              /* assign bitrate and weights */
              metric.face_br[EL_seg_id][viewport_tile[i]][layerId] =
                  meta.enha_layer_br[layerId - 1];
              cout << meta.enha_layer_br[layerId - 1] << endl;

              metric.face_ini_weight[EL_seg_id][viewport_tile[i]][layerId] = 20;

              /* send request */
              metric.face_stream_ID[EL_seg_id][viewport_tile[i]]
                                   [layerId] = send_get_request(
                  client, EL_seg_id, viewport_tile[i],
                  metric.face_br[EL_seg_id][viewport_tile[i]][layerId], 0,
                  metric.face_ini_weight[EL_seg_id][viewport_tile[i]][layerId]);
              cout
                  << "#[UTD2017_adaptation_vr] req_stream_id: "
                  << metric.face_stream_ID[EL_seg_id][viewport_tile[i]][layerId]
                  << "," << metric.face_br[EL_seg_id][viewport_tile[i]][layerId]
                  << endl;
              metric.iter_req_stream[iter_id].push_back(
                  metric.face_stream_ID[EL_seg_id][viewport_tile[i]][layerId]);

              /* map stream_id -> (index, faceid) */
              streamInfo = new StreamInfo(EL_seg_id, viewport_tile[i], layerId);
              metric.RequestStreamInfo.insert(std::pair<int, StreamInfo *>(
                  metric.face_stream_ID[EL_seg_id][viewport_tile[i]][layerId],
                  streamInfo));

              /* record request tiles */
              metric.iter_tile_list[iter_id].push_back(viewport_tile[i]);

              //
              req_s_num++;
            }
          }
        }
        fin_s_num = 0;
      }

      ////////// ALGORITHM 3 /////////////////
      if (meta.ALG == 3 || meta.ALG == 5) {
        // get number of pixels in viewport of each tile
        // Number of tile: meta.NO_TILE
        // List of tiles in viewport: viewport_tile
        pri = viewport_tile;
        tile_pixel_2 = player.vmask->get_pixel(cur_vp_pos);
        int h, t;
        cout << pri.size() << endl;
        cout << "pri = ";
        for (h = 0; h < pri.size(); h++) {
          cout << pri[h] << ", ";
        }
        cout << endl;
        cout << "tile pixel = ";
        for (t = 0; t < 25; t++) {
          cout << tile_pixel_2[t] << ", ";
        }
        cout << endl;

        metric.iter_down_time_budget[iter_id] =
            player.seg_playback_deadline[EL_seg_id] -
            (std::chrono::duration_cast<std::chrono::microseconds>(
                 std::chrono::steady_clock::now() -
                 client->timing.connect_end_time)
                 .count() /
             1000.0);
        cout << "time_budget: " << metric.iter_down_time_budget[iter_id]
             << endl;
        // Duc
        // Bubble sort viewport tile according to number of pixels
        bool swapped = false;
        int tmp;
        do {
          swapped = false;
          for (j = 0; j < pri.size() - 1; j++) {
            if (tile_pixel_2[pri[j] - 1] < tile_pixel_2[pri[j + 1] - 1]) {
              tmp = pri[j];
              pri[j] = pri[j + 1];
              pri[j + 1] = tmp;
              swapped = true;
            }
          }
        } while (swapped);

        cout << "pri = ";
        for (h = 0; h < pri.size(); h++) {
          cout << pri[h] << ", ";
          /* record request tiles */
          metric.iter_tile_list[iter_id].push_back(pri[h]);
        }
        cout << endl;

        /* calculate remaing time to download */
        metric.iter_down_time_budget[iter_id] =
            player.seg_playback_deadline[EL_seg_id] -
            (std::chrono::duration_cast<std::chrono::microseconds>(
                 std::chrono::steady_clock::now() -
                 client->timing.connect_end_time)
                 .count() /
             1000.0);
        cout << "time_budget: " << metric.iter_down_time_budget[iter_id]
             << endl;
        /* record selected tiles' versions */
        // metric.iter_tile_version[iter_id] = l;

        /* set maximum number of concurrent streams */
        nghttp2_settings_entry iv;
        iv.settings_id = NGHTTP2_SETTINGS_MAX_CONCURRENT_STREAMS;
        iv.value = 1;
        int niv = 1;
        int rv = nghttp2_submit_settings(client->session, NGHTTP2_FLAG_NONE,
                                         &iv, niv);
        if (rv != 0) {
          exit(-1);
        }

        sum_tile_br = meta.base_layer_br;
        cout << "ID = " << ID << endl;
        int dep_stream_id = 0;
        double alpha = 0.9;
        for (layerId = 1; layerId <= 2; layerId++) {
          ID = 0;
          while (ID < pri.size() &&
                 sum_tile_br + meta.enha_layer_br[layerId - 1] <
                     alpha * metric.iter_est_thrp[iter_id]) {

            /* assign bitrate and weights */
            metric.face_br[EL_seg_id][pri[ID]][layerId] =
                meta.enha_layer_br[layerId - 1];
            cout << meta.enha_layer_br[layerId - 1] << endl;

            metric.face_ini_weight[EL_seg_id][pri[ID]][layerId] = 20;

            /* send request */
            metric.face_stream_ID[EL_seg_id][pri[ID]][layerId] =
                send_get_request(
                    client, EL_seg_id, pri[ID],
                    metric.face_br[EL_seg_id][pri[ID]][layerId], dep_stream_id,
                    metric.face_ini_weight[EL_seg_id][pri[ID]][layerId]);
            dep_stream_id = metric.face_stream_ID[EL_seg_id][pri[ID]][layerId];

            cout << "###########[ALG 3] req_stream_id: "
                 << metric.face_stream_ID[EL_seg_id][pri[ID]][layerId] << ","
                 << pri[ID] << ","
                 << metric.face_br[EL_seg_id][pri[ID]][layerId] << endl;
            metric.iter_req_stream[iter_id].push_back(
                metric.face_stream_ID[EL_seg_id][pri[ID]][layerId]);

            /* map stream_id -> (index, faceid) */
            streamInfo = new StreamInfo(EL_seg_id, pri[ID], layerId);
            metric.RequestStreamInfo.insert(std::pair<int, StreamInfo *>(
                metric.face_stream_ID[EL_seg_id][pri[ID]][layerId],
                streamInfo));

            //
            req_s_num++;
            sum_tile_br += meta.enha_layer_br[layerId - 1];
            ID += 1;
            // std::this_thread::sleep_for(std::chrono::milliseconds(2));
          }
        }
        fin_s_num = 0;
      }
      // exit(1);

      if (meta.ALG == 1 || meta.ALG == 2) {

        /* find optimal version for visible tiles */
        for (l = 0; l < meta.EL_VER_NUM; l++) {
          if (meta.base_layer_br +
                  (EL_br_tmp + meta.enha_layer_br[l]) * viewport_tile.size() >
              metric.iter_est_thrp[iter_id])
            break;
          EL_br_tmp += meta.enha_layer_br[l];
        }

        cout << "#[UTD2017]: Enhan. layer: " << l << ","
             << metric.iter_est_thrp[iter_id] << "," << viewport_tile.size()
             << endl;

        /* send request for visible tiles */
        if (l > 0) {
          /* calculate remaing time to download */
          metric.iter_down_time_budget[iter_id] =
              player.seg_playback_deadline[EL_seg_id] -
              (std::chrono::duration_cast<std::chrono::microseconds>(
                   std::chrono::steady_clock::now() -
                   client->timing.connect_end_time)
                   .count() /
               1000.0);

          /* record selected tiles' versions */
          metric.iter_tile_version[iter_id] = l;

          /* set maximum number of concurrent streams */
          nghttp2_settings_entry iv;
          iv.settings_id = NGHTTP2_SETTINGS_MAX_CONCURRENT_STREAMS;
          iv.value = 1;
          int niv = 1;
          int rv = nghttp2_submit_settings(client->session, NGHTTP2_FLAG_NONE,
                                           &iv, niv);
          if (rv != 0) {
            exit(-1);
          }
          int dep_stream_id = 0;
          /* send get requests */
          for (i = 0; i < viewport_tile.size(); i++) {
            for (layerId = 1; layerId <= l; layerId++) {

              /* assign bitrate and weights */
              metric.face_br[EL_seg_id][viewport_tile[i]][layerId] =
                  meta.enha_layer_br[layerId - 1];
              cout << meta.enha_layer_br[layerId - 1] << endl;

              metric.face_ini_weight[EL_seg_id][viewport_tile[i]][layerId] = 20;

              /* send request */
              metric.face_stream_ID[EL_seg_id][viewport_tile[i]]
                                   [layerId] = send_get_request(
                  client, EL_seg_id, viewport_tile[i],
                  metric.face_br[EL_seg_id][viewport_tile[i]][layerId],
                  dep_stream_id,
                  metric.face_ini_weight[EL_seg_id][viewport_tile[i]][layerId]);
              dep_stream_id =
                  metric.face_stream_ID[EL_seg_id][viewport_tile[i]][layerId];

              cout
                  << "#[UTD2017_adaptation_vr] req_stream_id: "
                  << metric.face_stream_ID[EL_seg_id][viewport_tile[i]][layerId]
                  << "," << metric.face_br[EL_seg_id][viewport_tile[i]][layerId]
                  << endl;
              metric.iter_req_stream[iter_id].push_back(
                  metric.face_stream_ID[EL_seg_id][viewport_tile[i]][layerId]);

              /* map stream_id -> (index, faceid) */
              streamInfo = new StreamInfo(EL_seg_id, viewport_tile[i], layerId);
              metric.RequestStreamInfo.insert(std::pair<int, StreamInfo *>(
                  metric.face_stream_ID[EL_seg_id][viewport_tile[i]][layerId],
                  streamInfo));

              /* record request tiles */
              metric.iter_tile_list[iter_id].push_back(viewport_tile[i]);

              //
              req_s_num++;
            }
          }

          /* update status */
          // req_s_num = viewport_tile.size() * l;
          fin_s_num = 0;
        }
      }
    }
  }
  if (req_s_num > 0) {
    /* init status of this round */
    metric.iter_down_data[iter_id] = 0;
    metric.iter_down_data_frame[iter_id] = 0;
    metric.iter_down_time[iter_id] = 0;
  } else {
    /* wait for 100ms, then repeat decision engine */
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    UTD2017_adaptation_vr(client, iter_id);
  }
}

// }
//-----------------------------------------------
//----------------------------------------------
int check_tile_in_region(int tile) {
  for (int i = 0; i < region1.size(); i++)
    if (region1.at(i) == tile)
      return 1;
  for (int i = 0; i < region2.size(); i++)
    if (region2.at(i) == tile)
      return 2;
  return 3;
}

void delay(struct ev_loop *loop, ev_timer *w, int revents) {
  cout << "Delay for request" << endl;
}

namespace {
int on_stream_close_callback(nghttp2_session *session, int32_t stream_id,
                             uint32_t error_code, void *user_data) {
  bool IS_BASE_LAYER = false;
  int sid, fid, layerId;
  auto client = get_client(user_data);
  auto req = static_cast<Request *>(
      nghttp2_session_get_stream_user_data(session, stream_id));

  // cout << "#######################[on_stream_close] " << stream_id <<
  // ",error_code:" << error_code << endl;
  // curr_session = session;
  if (!req) {
    return 0;
  }

  // If this request is using Expect/Continue, stop its ContinueTimer.
  if (req->continue_timer) {
    req->continue_timer->stop();
  }

  update_html_parser(client, req, nullptr, 0, 1);
  ++client->complete;

  /* start adaptation */
  if (stream_id == INI_STREAM_ID) {
    cur_req_stream_id = stream_id;
    BL_seg_id = 0;
    player.connect_end_time = client->timing.connect_end_time;
    if (meta.ALG == 5 || meta.ALG == 6 || meta.ALG == 7 || meta.ALG == 8 ||
        meta.ALG == 9 || meta.ALG == 10 || meta.ALG == 11 || meta.ALG == 12 ||
        meta.ALG == 13) {
      MMSP(client, iter_id);
    } else {
      UTD2017_adaptation_vr(client, iter_id);
    }
  }

  // if(metric.push_stream_id_2_sid.find(stream_id) !=
  // metric.push_stream_id_2_sid.end()){
  if (metric.pushStreamInfo.find(stream_id) != metric.pushStreamInfo.end()) {

    /* Determine segment/face/layer ids */
    //
    sid = metric.pushStreamInfo.find(stream_id)->second->sid;
    fid = metric.pushStreamInfo.find(stream_id)->second->fid;
    layerId = metric.pushStreamInfo.find(stream_id)->second->lid;
    cout << "######[on_stream_close] push stream closed: #" << stream_id << ","
         << sid << "," << fid << "," << layerId << endl;

    /* update finished stream */
    metric.iter_push_stream_fin[iter_id].push_back(stream_id);
  }

  /* record metrics of each stream */
  if (metric.RequestStreamInfo.find(stream_id) !=
      metric.RequestStreamInfo.end()) {

    /* Determine segment/face/layer ids */
    sid = metric.RequestStreamInfo.find(stream_id)->second->sid;
    fid = metric.RequestStreamInfo.find(stream_id)->second->fid;
    layerId = metric.RequestStreamInfo.find(stream_id)->second->lid;

    /* update finished request stream */
    metric.iter_req_stream_fin[iter_id].push_back(stream_id);

    cout << "######[on_stream_close] stream_id #" << stream_id << "," << sid
         << "," << fid << "," << layerId << endl;

    /* calculate stream-realted metrics */
    // request time
    metric.face_req_time[sid][fid][layerId] =
        std::chrono::duration_cast<std::chrono::microseconds>(
            req->timing.request_start_time - client->timing.connect_end_time)
            .count() /
        1000.0;

    // complete time
    metric.face_end_time[sid][fid][layerId] =
        std::chrono::duration_cast<std::chrono::microseconds>(
            req->timing.response_end_time - client->timing.connect_end_time)
            .count() /
        1000.0;

    // download time
    metric.face_dow_time[sid][fid][layerId] =
        metric.face_end_time[sid][fid][layerId] -
        metric.face_req_time[sid][fid][layerId];

    // download throughput
    metric.face_dow_thrp[sid][fid][layerId] =
        metric.face_dow_data[sid][fid][layerId] /
        (metric.face_dow_time[sid][fid][layerId] / 1000.0);
    // cout <<  metric.face_req_time[sid][fid][layerId] << "," <<
    // metric.face_end_time[sid][fid][layerId] << "," <<
    // metric.face_dow_time[sid][fid][layerId] << "," <<
    // metric.face_dow_data[sid][fid][layerId] << "," <<
    // metric.face_dow_thrp[sid][fid][layerId] << endl;

    // tile size
    metric.face_size[sid][fid][layerId] =
        metric.face_dow_data[sid][fid][layerId];

    /* check if the tile has been cancelled */
    if (!cancelled_request_stream.empty() &&
        std::find(cancelled_request_stream.begin(),
                  cancelled_request_stream.end(),
                  stream_id) != cancelled_request_stream.end()) {
      // does not count stream in the 'remove' vtile set
      metric.face_cancel[sid][fid][layerId] = 1;
      cout << "#######[on_stream_close] Cancelled stream " << endl;
    } else {
      metric.face_cancel[sid][fid][layerId] = 0;
    }
    fin_s_num++;

    /* Check if this tile arrives after its playback deadline */
    if (metric.face_end_time[sid][fid][layerId] >
        player.seg_playback_deadline[sid])
      metric.face_late[sid][fid][layerId] = 1;
    else
      metric.face_late[sid][fid][layerId] = 0;

    cout << "######[on_stream_close] fin_s_num: " << fin_s_num
         << ", req_s_num: " << req_s_num << endl;

    /* update tiles' versions only if the tile is neither a late tile nor
     * canceled tile */
    if (metric.face_late[sid][fid][layerId] == 0 &&
        metric.face_cancel[sid][fid][layerId] == 0)
      player.buff.update_tile_version(sid, fid, layerId);

    /* write stream info to file */
    metric.print(sid, fid, layerId);
    metric.print_to_file(log_stream, iter_id, sid, fid, layerId);

    /* update status of this iteration*/
    metric.iter_down_data[iter_id] += metric.face_dow_data[sid][fid][layerId];

    /* all segments of this round arrived, then move to the next round */
    if (fin_s_num == req_s_num) {

      /* calculate avearge thoughput of this iteration */
      metric.iter_end_time[iter_id] =
          std::chrono::duration_cast<std::chrono::microseconds>(
              get_time() - client->timing.connect_end_time)
              .count() /
          1000.0;
      metric.iter_down_time[iter_id] =
          metric.iter_end_time[iter_id] - metric.iter_start_time[iter_id];
      metric.iter_sam_thrp[iter_id] = metric.iter_down_data[iter_id] /
                                      (metric.iter_down_time[iter_id] / 1000.0);
      /* mark segment with ELs downloaded --> avoid repeated downloads on the
       * same segment */
      if (fid > 0) {
        metric.seg_EL_download[sid] = true;
      }
      /* update base layer bufer (never cancel a base layer) */
      if (fid == 0) {
        player.incre_buff_size(meta.SD * 1000);
        metric.base_buf_sec[BL_seg_id] = player.get_cur_buff_size_second();
        BL_seg_id++;
      }
      /* write iteration info to file */
      metric.iter_base_buff[iter_id] = player.get_cur_buff_size_second();
      fprintf(log_iter, "%d,%.2f,%.2f,%.2f,%.2f,%.2f,%d,%.2f,%.2f\n", iter_id,
              metric.iter_start_time[iter_id], metric.iter_end_time[iter_id],
              metric.iter_sam_thrp[iter_id], metric.iter_est_thrp[iter_id],
              metric.iter_base_buff[iter_id], req_s_num,
              metric.iter_down_time_budget[iter_id],
              metric.iter_smooth_bitrate[iter_id]);
      fflush(log_iter);

      /* mark finish of this iteration */
      metric.mark_end_iter(iter_id);

      /* Terminate session if all segment have been downloaded */
      // if(sid == (meta.NO_SEG -1) && fid > 0)
      //   nghttp2_session_terminate_session(session, NGHTTP2_NO_ERROR);

      /* go to next iteration */
      iter_id++;
      if (meta.ALG == 5 || meta.ALG == 6 || meta.ALG == 7 || meta.ALG == 8 ||
          meta.ALG == 9 || meta.ALG == 10 || meta.ALG == 11 || meta.ALG == 12 ||
          meta.ALG == 13) {
        MMSP(client, iter_id);
      } else {
        UTD2017_adaptation_vr(client, iter_id);
      }
    }
  }

  return 0;
}
} // namespace

struct RequestResult {
  std::chrono::microseconds time;
};

namespace {
void print_stats(const HttpClient &client) {
  std::cout << "***** Statistics *****" << std::endl;
  ofstream f[num_of_tile];
  ofstream buffer;
  ofstream dang_file;
  // ofstream f2;
  std::vector<Request *> reqs;
  reqs.reserve(client.reqvec.size());
  for (const auto &req : client.reqvec) {
    if (req->timing.state == RequestState::ON_COMPLETE) {
      reqs.push_back(req.get());
    }
  }
  buffering_file.close();
  dang_file.open("/home/haidang/client/Output/results.txt");
  dang_file << "finishTime\tSeg\tTile\tThrp\tTile rate\tSegment rate\tBuffer"
            << endl;
  std::sort(std::begin(reqs), std::end(reqs),
            [](const Request *lhs, const Request *rhs) {
              const auto &ltiming = lhs->timing;
              const auto &rtiming = rhs->timing;
              return ltiming.response_end_time < rtiming.response_end_time ||
                     (ltiming.response_end_time == rtiming.response_end_time &&
                      ltiming.request_start_time < rtiming.request_start_time);
            });

  std::cout << R"(
Request timing:
  responseEnd: the  time  when  last  byte of  response  was  received
               relative to connectEnd
 requestStart: the time  just before  first byte  of request  was sent
               relative  to connectEnd.   If  '*' is  shown, this  was
               pushed by server.
      process: responseEnd - requestStart
         code: HTTP status code
         size: number  of  bytes  received as  response  body  without
               inflation.
          URI: request URI

see http://www.w3.org/TR/resource-timing/#processing-model

sorted by 'complete'

id  responseEnd responseStart requestStart  process code size request path)"
            << std::endl;

  // Hung:
  int index[num_of_tile];
  int curr_seg = 0;
  double thrp_print;
  int cnt = 0, cnt_buff = 0;
  for (int i = 0; i < num_of_tile; i++)
    index[i] = 0;
  const auto &base = client.timing.connect_end_time;
  for (const auto &req : reqs) {
    auto response_end = std::chrono::duration_cast<std::chrono::microseconds>(
        req->timing.response_end_time - base);
    auto response_start = std::chrono::duration_cast<std::chrono::microseconds>(
        req->timing.response_start_time - base);
    auto request_start = std::chrono::duration_cast<std::chrono::microseconds>(
        req->timing.request_start_time - base);
    auto total = std::chrono::duration_cast<std::chrono::microseconds>(
        req->timing.response_end_time - req->timing.request_start_time);
    auto pushed = req->stream_id % 2 == 0;

    std::cout << std::setw(3) << req->stream_id << " " << std::setw(11)
              << ("+" + util::format_duration(response_end)) << " "
              << std::setw(13) << ("+" + util::format_duration(response_start))
              << " " << (pushed ? "*" : " ") << std::setw(11)
              << ("+" + util::format_duration(request_start)) << " "
              << std::setw(8) << util::format_duration(total) << " "
              << std::setw(4) << req->status << " " << std::setw(4)
              << util::utos_unit(req->response_len) << " "
              << req->make_reqpath() << std::endl;
    if (req->stream_id % 2 == 0) {

      // if(dang_get_face_from_uri(req->make_reqpath())==num_of_tile){
      // thrp_print = dang_thrp_record.at(cnt);
      cnt++;
      // // }
      dang_file << response_end.count() * 1.0 / 1000000 << '\t'
                << dang_get_seg_from_uri(req->make_reqpath()) << '\t'
                << dang_get_face_from_uri(req->make_reqpath()) << '\t'

                << thrp_print << '\t' << req->response_len * 8.0 / (dang_sd)
                << '\t'
                << dang_segment_rate.at(
                       dang_get_seg_from_uri(req->make_reqpath()) - 1)
                << '\t' << endl;
      cnt_buff++;
    }
  }

  dang_file.close();
  if (system("sudo killall complex_.sh"))
    cout << "Kill DummyNet";
  cout << "finish" << endl;
}

} // namespace

namespace {
int client_select_next_proto_cb(SSL *ssl, unsigned char **out,
                                unsigned char *outlen, const unsigned char *in,
                                unsigned int inlen, void *arg) {
  if (config.verbose) {
    print_timer();
    std::cout << "[NPN] server offers:" << std::endl;
  }
  for (unsigned int i = 0; i < inlen; i += in[i] + 1) {
    if (config.verbose) {
      std::cout << "          * ";
      std::cout.write(reinterpret_cast<const char *>(&in[i + 1]), in[i]);
      std::cout << std::endl;
    }
  }
  if (!util::select_h2(const_cast<const unsigned char **>(out), outlen, in,
                       inlen)) {
    print_protocol_nego_error();
    return SSL_TLSEXT_ERR_NOACK;
  }
  return SSL_TLSEXT_ERR_OK;
}
} // namespace

namespace {
// Recommended general purpose "Intermediate compatibility" cipher by
// mozilla.
//
// https://wiki.mozilla.org/Security/Server_Side_TLS
const char *const CIPHER_LIST =
    "ECDHE-RSA-AES128-GCM-SHA256:ECDHE-ECDSA-AES128-GCM-SHA256:ECDHE-RSA-"
    "AES256-GCM-SHA384:ECDHE-ECDSA-AES256-GCM-SHA384:DHE-RSA-AES128-GCM-SHA256:"
    "DHE-DSS-AES128-GCM-SHA256:kEDH+AESGCM:ECDHE-RSA-AES128-SHA256:ECDHE-ECDSA-"
    "AES128-SHA256:ECDHE-RSA-AES128-SHA:ECDHE-ECDSA-AES128-SHA:ECDHE-RSA-"
    "AES256-SHA384:ECDHE-ECDSA-AES256-SHA384:ECDHE-RSA-AES256-SHA:ECDHE-ECDSA-"
    "AES256-SHA:DHE-RSA-AES128-SHA256:DHE-RSA-AES128-SHA:DHE-DSS-AES128-SHA256:"
    "DHE-RSA-AES256-SHA256:DHE-DSS-AES256-SHA:DHE-RSA-AES256-SHA:AES128-GCM-"
    "SHA256:AES256-GCM-SHA384:AES128-SHA256:AES256-SHA256:AES128-SHA:AES256-"
    "SHA:AES:CAMELLIA:DES-CBC3-SHA:!aNULL:!eNULL:!EXPORT:!DES:!RC4:!MD5:!PSK:!"
    "aECDH:!EDH-DSS-DES-CBC3-SHA:!EDH-RSA-DES-CBC3-SHA:!KRB5-DES-CBC3-SHA";
} // namespace

namespace {
int communicate(
    const std::string &scheme, const std::string &host, uint16_t port,
    std::vector<
        std::tuple<std::string, nghttp2_data_provider *, int64_t, int32_t>>
        requests,
    const nghttp2_session_callbacks *callbacks) {
  int result = 0;
  auto loop = EV_DEFAULT;
  SSL_CTX *ssl_ctx = nullptr;
  if (scheme == "https") {
    ssl_ctx = SSL_CTX_new(SSLv23_client_method());
    if (!ssl_ctx) {
      std::cerr << "[ERROR] Failed to create SSL_CTX: "
                << ERR_error_string(ERR_get_error(), nullptr) << std::endl;
      result = -1;
      goto fin;
    }

    auto ssl_opts = (SSL_OP_ALL & ~SSL_OP_DONT_INSERT_EMPTY_FRAGMENTS) |
                    SSL_OP_NO_SSLv2 | SSL_OP_NO_SSLv3 | SSL_OP_NO_COMPRESSION |
                    SSL_OP_NO_SESSION_RESUMPTION_ON_RENEGOTIATION;

    SSL_CTX_set_options(ssl_ctx, ssl_opts);
    SSL_CTX_set_mode(ssl_ctx, SSL_MODE_AUTO_RETRY);
    SSL_CTX_set_mode(ssl_ctx, SSL_MODE_RELEASE_BUFFERS);
    if (SSL_CTX_set_cipher_list(ssl_ctx, CIPHER_LIST) == 0) {
      std::cerr << "[ERROR] " << ERR_error_string(ERR_get_error(), nullptr)
                << std::endl;
      result = -1;
      goto fin;
    }
    if (!config.keyfile.empty()) {
      if (SSL_CTX_use_PrivateKey_file(ssl_ctx, config.keyfile.c_str(),
                                      SSL_FILETYPE_PEM) != 1) {
        std::cerr << "[ERROR] " << ERR_error_string(ERR_get_error(), nullptr)
                  << std::endl;
        result = -1;
        goto fin;
      }
    }
    if (!config.certfile.empty()) {
      if (SSL_CTX_use_certificate_chain_file(ssl_ctx,
                                             config.certfile.c_str()) != 1) {
        std::cerr << "[ERROR] " << ERR_error_string(ERR_get_error(), nullptr)
                  << std::endl;
        result = -1;
        goto fin;
      }
    }
    SSL_CTX_set_next_proto_select_cb(ssl_ctx, client_select_next_proto_cb,
                                     nullptr);

#if OPENSSL_VERSION_NUMBER >= 0x10002000L
    auto proto_list = util::get_default_alpn();

    SSL_CTX_set_alpn_protos(ssl_ctx, proto_list.data(), proto_list.size());
#endif // OPENSSL_VERSION_NUMBER >= 0x10002000L
  }
  {
    HttpClient client{callbacks, loop, ssl_ctx};

    int32_t dep_stream_id = 0;

    if (!config.no_dep) {
      dep_stream_id = anchors[ANCHOR_FOLLOWERS].stream_id;
    }

    for (auto &req : requests) {
      nghttp2_priority_spec pri_spec;

      nghttp2_priority_spec_init(&pri_spec, dep_stream_id, std::get<3>(req), 0);

      for (int i = 0; i < config.multiply; ++i) {
        // client.add_request(std::get<0>(req), std::get<1>(req),
        // std::get<2>(req),
        //                   pri_spec);

        // Hung_comment: These parameters are too complicated thus saving them
        // is the best solution
        dang_data_prd = std::get<1>(req);
        dang_data_length = std::get<2>(req);
        dang_pri_spec = pri_spec;
        client.add_request(std::get<0>(req), std::get<1>(req), std::get<2>(req),
                           pri_spec);

        // hung_req_vod_rebuff(&client, false);
        // break;
      }
    }
    // request_corr_id[13]=1;
    // request_corr_id[15]=2;
    client.update_hostport();

    client.record_start_time();

    if (client.resolve_host(host, port) != 0) {
      goto fin;
    }

    client.record_domain_lookup_end_time();

    if (client.initiate_connection() != 0) {
      std::cerr << "[ERROR] Could not connect to " << host << ", port " << port
                << std::endl;
      goto fin;
    }

    ev_set_userdata(loop, &client);
    ev_run(loop, 0);
    ev_set_userdata(loop, nullptr);

#ifdef HAVE_JANSSON
    if (!config.harfile.empty()) {
      FILE *outfile;
      if (config.harfile == "-") {
        outfile = stdout;
      } else {
        outfile = fopen(config.harfile.c_str(), "wb");
      }

      if (outfile) {
        client.output_har(outfile);

        if (outfile != stdout) {
          fclose(outfile);
        }
      } else {
        std::cerr << "Cannot open file " << config.harfile << ". "
                  << "har file could not be created." << std::endl;
      }
    }
#endif // HAVE_JANSSON

    if (client.success != client.reqvec.size()) {
      std::cerr << "Some requests were not processed. total="
                << client.reqvec.size() << ", processed=" << client.success
                << std::endl;
    }
    if (config.stat) {
      print_stats(client);
    }
  }
fin:
  if (ssl_ctx) {
    SSL_CTX_free(ssl_ctx);
  }
  // cout<<"finish"<<endl;
  return result;
}
} // namespace

namespace {
ssize_t file_read_callback(nghttp2_session *session, int32_t stream_id,
                           uint8_t *buf, size_t length, uint32_t *data_flags,
                           nghttp2_data_source *source, void *user_data) {
  int rv;
  auto req = static_cast<Request *>(
      nghttp2_session_get_stream_user_data(session, stream_id));
  assert(req);
  int fd = source->fd;
  ssize_t nread;

  while ((nread = pread(fd, buf, length, req->data_offset)) == -1 &&
         errno == EINTR)
    ;

  if (nread == -1) {
    return NGHTTP2_ERR_TEMPORAL_CALLBACK_FAILURE;
  }

  req->data_offset += nread;

  if (req->data_offset == req->data_length) {
    *data_flags |= NGHTTP2_DATA_FLAG_EOF;
    if (!config.trailer.empty()) {
      std::vector<nghttp2_nv> nva;
      nva.reserve(config.trailer.size());
      for (auto &kv : config.trailer) {
        nva.push_back(http2::make_nv(kv.name, kv.value, kv.no_index));
      }
      rv = nghttp2_submit_trailer(session, stream_id, nva.data(), nva.size());
      if (rv != 0) {
        if (nghttp2_is_fatal(rv)) {
          return NGHTTP2_ERR_CALLBACK_FAILURE;
        }
      } else {
        *data_flags |= NGHTTP2_DATA_FLAG_NO_END_STREAM;
      }
    }

    return nread;
  }

  if (req->data_offset > req->data_length || nread == 0) {
    return NGHTTP2_ERR_TEMPORAL_CALLBACK_FAILURE;
  }

  return nread;
}
} // namespace

namespace {
int run(char **uris, int n) {
  nghttp2_session_callbacks *callbacks;

  nghttp2_session_callbacks_new(&callbacks);
  auto cbsdel = defer(nghttp2_session_callbacks_del, callbacks);

  nghttp2_session_callbacks_set_on_stream_close_callback(
      callbacks, on_stream_close_callback);

  nghttp2_session_callbacks_set_on_frame_recv_callback(callbacks,
                                                       on_frame_recv_callback2);

  if (config.verbose) {
    nghttp2_session_callbacks_set_on_invalid_frame_recv_callback(
        callbacks, verbose_on_invalid_frame_recv_callback);

    nghttp2_session_callbacks_set_error_callback(callbacks,
                                                 verbose_error_callback);
  }

  nghttp2_session_callbacks_set_on_data_chunk_recv_callback(
      callbacks, on_data_chunk_recv_callback);

  nghttp2_session_callbacks_set_on_begin_headers_callback(
      callbacks, on_begin_headers_callback);

  nghttp2_session_callbacks_set_on_header_callback(callbacks,
                                                   on_header_callback);

  nghttp2_session_callbacks_set_before_frame_send_callback(
      callbacks, before_frame_send_callback);

  nghttp2_session_callbacks_set_on_frame_send_callback(callbacks,
                                                       on_frame_send_callback);

  nghttp2_session_callbacks_set_on_frame_not_send_callback(
      callbacks, on_frame_not_send_callback);

  if (config.padding) {
    nghttp2_session_callbacks_set_select_padding_callback(
        callbacks, select_padding_callback);
  }

  std::string prev_scheme;
  std::string prev_host;
  uint16_t prev_port = 0;
  int failures = 0;
  int data_fd = -1;
  nghttp2_data_provider data_prd;
  struct stat data_stat;

  if (!config.datafile.empty()) {
    if (config.datafile == "-") {
      if (fstat(0, &data_stat) == 0 &&
          (data_stat.st_mode & S_IFMT) == S_IFREG) {
        // use STDIN if it is a regular file
        data_fd = 0;
      } else {
        // copy the contents of STDIN to a temporary file
        char tempfn[] = "/tmp/nghttp.temp.XXXXXX";
        data_fd = mkstemp(tempfn);
        if (data_fd == -1) {
          std::cerr << "[ERROR] Could not create a temporary file in /tmp"
                    << std::endl;
          return 1;
        }
        if (unlink(tempfn) != 0) {
          std::cerr << "[WARNING] failed to unlink temporary file:" << tempfn
                    << std::endl;
        }
        while (1) {
          std::array<char, 1_k> buf;
          ssize_t rret, wret;
          while ((rret = read(0, buf.data(), buf.size())) == -1 &&
                 errno == EINTR)
            ;
          if (rret == 0)
            break;
          if (rret == -1) {
            std::cerr << "[ERROR] I/O error while reading from STDIN"
                      << std::endl;
            return 1;
          }
          while ((wret = write(data_fd, buf.data(), rret)) == -1 &&
                 errno == EINTR)
            ;
          if (wret != rret) {
            std::cerr << "[ERROR] I/O error while writing to temporary file"
                      << std::endl;
            return 1;
          }
        }
        if (fstat(data_fd, &data_stat) == -1) {
          close(data_fd);
          std::cerr << "[ERROR] Could not stat temporary file" << std::endl;
          return 1;
        }
      }
    } else {
      data_fd = open(config.datafile.c_str(), O_RDONLY | O_BINARY);
      if (data_fd == -1) {
        std::cerr << "[ERROR] Could not open file " << config.datafile
                  << std::endl;
        return 1;
      }
      if (fstat(data_fd, &data_stat) == -1) {
        close(data_fd);
        std::cerr << "[ERROR] Could not stat file " << config.datafile
                  << std::endl;
        return 1;
      }
    }
    data_prd.source.fd = data_fd;
    data_prd.read_callback = file_read_callback;
  }
  std::vector<
      std::tuple<std::string, nghttp2_data_provider *, int64_t, int32_t>>
      requests;

  size_t next_weight_idx = 0;

  for (int i = 0; i < n; ++i) {
    http_parser_url u{};
    auto uri = strip_fragment(uris[i]);
    if (http_parser_parse_url(uri.c_str(), uri.size(), 0, &u) != 0) {
      ++next_weight_idx;
      std::cerr << "[ERROR] Could not parse URI " << uri << std::endl;
      continue;
    }
    if (!util::has_uri_field(u, UF_SCHEMA)) {
      ++next_weight_idx;
      std::cerr << "[ERROR] URI " << uri << " does not have scheme part"
                << std::endl;
      continue;
    }
    auto port = util::has_uri_field(u, UF_PORT)
                    ? u.port
                    : util::get_default_port(uri.c_str(), u);
    auto host = decode_host(util::get_uri_field(uri.c_str(), u, UF_HOST));

    // Hung: get defaul URI, and starting bitrate
    auto dang_scheme = util::get_uri_field(uri.c_str(), u, UF_SCHEMA).str();
    dang_uri = dang_scheme + "://" + host + ":" + std::to_string(port);

    if (!util::fieldeq(uri.c_str(), u, UF_SCHEMA, prev_scheme.c_str()) ||
        host != prev_host || port != prev_port) {
      if (!requests.empty()) {
        if (communicate(prev_scheme, prev_host, prev_port, std::move(requests),
                        callbacks) != 0) {
          ++failures;
        }
        requests.clear();
      }
      prev_scheme = util::get_uri_field(uri.c_str(), u, UF_SCHEMA).str();
      prev_host = std::move(host);
      prev_port = port;
    }
    requests.emplace_back(uri, data_fd == -1 ? nullptr : &data_prd,
                          data_stat.st_size, config.weight[next_weight_idx++]);

    // Hung:
    for (int i = 0; i < hung_time_recorder.size(); i++) {
      // std::cout << hung_seg_recorder.at(i) << "\t"
      //           << hung_time_recorder.at(i)/1000.0 << "\t"
      //           << hung_thrp_recorder.at(i) << "\t"
      //           << hung_rate_recorder.at(i) << "\t"
      //           << hung_buff_recorder.at(i)/1000.0 << "\t"
      //           << std::endl;
    }
  }
  if (!requests.empty()) {
    if (communicate(prev_scheme, prev_host, prev_port, std::move(requests),
                    callbacks) != 0) {
      ++failures;
    }
  }
  cout << "ok" << endl;
  return failures;
}
} // namespace

namespace {
void print_version(std::ostream &out) {
  out << "nghttp nghttp2/" NGHTTP2_VERSION << std::endl;
}
} // namespace

namespace {
void print_usage(std::ostream &out) {
  out << R"(Usage: nghttp [OPTIONS]... <URI>...
HTTP/2 client)"
      << std::endl;
}
} // namespace

namespace {
void print_help(std::ostream &out) {
  print_usage(out);
  out << R"(
  <URI>       Specify URI to access.
Options:
  -v, --verbose
              Print   debug   information   such  as   reception   and
              transmission of frames and name/value pairs.  Specifying
              this option multiple times increases verbosity.
  -n, --null-out
              Discard downloaded data.
  -O, --remote-name
              Save  download  data  in  the  current  directory.   The
              filename is  derived from  URI.  If  URI ends  with '/',
              'index.html'  is used  as a  filename.  Not  implemented
              yet.
  -t, --timeout=<DURATION>
              Timeout each request after <DURATION>.  Set 0 to disable
              timeout.
  -w, --window-bits=<N>
              Sets the stream level initial window size to 2**<N>-1.
  -W, --connection-window-bits=<N>
              Sets  the  connection  level   initial  window  size  to
              2**<N>-1.
  -a, --get-assets
              Download assets  such as stylesheets, images  and script
              files linked  from the downloaded resource.   Only links
              whose  origins are  the same  with the  linking resource
              will be downloaded.   nghttp prioritizes resources using
              HTTP/2 dependency  based .  The  priority order,
              from highest to lowest,  is html itself, css, javascript
              and images.
  -s, --stat  Print statistics.
  -H, --header=<HEADER>
              Add a header to the requests.  Example: -H':method: PUT'
  --trailer=<HEADER>
              Add a trailer header to the requests.  <HEADER> must not
              include pseudo header field  (header field name starting
              with ':').  To  send trailer, one must use  -d option to
              send request body.  Example: --trailer 'foo: bar'.
  --cert=<CERT>
              Use  the specified  client certificate  file.  The  file
              must be in PEM format.
  --key=<KEY> Use the  client private key  file.  The file must  be in
              PEM format.
  -d, --data=<PATH>
              Post FILE to server. If '-'  is given, data will be read
              from stdin.
  -m, --multiply=<N>
              Request each URI <N> times.  By default, same URI is not
              requested twice.  This option disables it too.
  -u, --upgrade
              Perform HTTP Upgrade for HTTP/2.  This option is ignored
              if the request URI has https scheme.  If -d is used, the
              HTTP upgrade request is performed with OPTIONS method.
  -p, --weight=<WEIGHT>
              Sets  weight of  given  URI.  This  option  can be  used
              multiple times, and  N-th -p option sets  weight of N-th
              URI in the command line.  If  the number of -p option is
              less than the number of URI, the last -p option value is
              repeated.  If there is no -p option, default weight, 16,
              is assumed.  The valid value range is
              [)"
      << NGHTTP2_MIN_WEIGHT << ", " << NGHTTP2_MAX_WEIGHT << R"(], inclusive.
  -M, --peer-max-concurrent-streams=<N>
              Use  <N>  as  SETTINGS_MAX_CONCURRENT_STREAMS  value  of
              remote endpoint as if it  is received in SETTINGS frame.
              Default: 100
  -c, --header-table-size=<SIZE>
              Specify decoder  header table  size.  If this  option is
              used  multiple times,  and the  minimum value  among the
              given values except  for last one is  strictly less than
              the last  value, that minimum  value is set  in SETTINGS
              frame  payload  before  the   last  value,  to  simulate
              multiple header table size change.
  --encoder-header-table-size=<SIZE>
              Specify encoder header table size.  The decoder (server)
              specifies  the maximum  dynamic table  size it  accepts.
              Then the negotiated dynamic table size is the minimum of
              this option value and the value which server specified.
  -b, --padding=<N>
              Add at  most <N>  bytes to a  frame payload  as padding.
              Specify 0 to disable padding.
  -r, --har=<PATH>
              Output HTTP  transactions <PATH> in HAR  format.  If '-'
              is given, data is written to stdout.
  --color     Force colored log output.
  --continuation
              Send large header to test CONTINUATION.
  --no-content-length
              Don't send content-length header field.
  --no-dep    Don't send dependency based priority hint to server.
  --hexdump   Display the  incoming traffic in  hexadecimal (Canonical
              hex+ASCII display).  If SSL/TLS  is used, decrypted data
              are used.
  --no-push   Disable server push.
  --max-concurrent-streams=<N>
              The  number of  concurrent  pushed  streams this  client
              accepts.
  --expect-continue
              Perform an Expect/Continue handshake:  wait to send DATA
              (up to  a short  timeout)  until the server sends  a 100
              Continue interim response. This option is ignored unless
              combined with the -d option.
  --version   Display version information and exit.
  -h, --help  Display this help and exit.

--

  The <SIZE> argument is an integer and an optional unit (e.g., 10K is
  10 * 1024).  Units are K, M and G (powers of 1024).

  The <DURATION> argument is an integer and an optional unit (e.g., 1s
  is 1 second and 500ms is 500 milliseconds).  Units are h, m, s or ms
  (hours, minutes, seconds and milliseconds, respectively).  If a unit
  is omitted, a second is used as unit.)"
      << std::endl;
}
} // namespace

int main(int argc, char **argv) {
  ssl::libssl_init();

  bool color = false;
  while (1) {
    static int flag = 0;
    static option long_options[] = {
        {"verbose", no_argument, nullptr, 'v'},
        {"null-out", no_argument, nullptr, 'n'},
        {"remote-name", no_argument, nullptr, 'O'},
        {"timeout", required_argument, nullptr, 't'},
        {"window-bits", required_argument, nullptr, 'w'},
        {"connection-window-bits", required_argument, nullptr, 'W'},
        {"get-assets", no_argument, nullptr, 'a'},
        {"stat", no_argument, nullptr, 's'},
        {"help", no_argument, nullptr, 'h'},
        {"header", required_argument, nullptr, 'H'},
        {"data", required_argument, nullptr, 'd'},
        {"multiply", required_argument, nullptr, 'm'},
        {"upgrade", no_argument, nullptr, 'u'},
        {"weight", required_argument, nullptr, 'p'},
        {"peer-max-concurrent-streams", required_argument, nullptr, 'M'},
        {"header-table-size", required_argument, nullptr, 'c'},
        {"padding", required_argument, nullptr, 'b'},
        {"har", required_argument, nullptr, 'r'},
        {"cert", required_argument, &flag, 1},
        {"key", required_argument, &flag, 2},
        {"color", no_argument, &flag, 3},
        {"continuation", no_argument, &flag, 4},
        {"version", no_argument, &flag, 5},
        {"no-content-length", no_argument, &flag, 6},
        {"no-dep", no_argument, &flag, 7},
        {"trailer", required_argument, &flag, 9},
        {"hexdump", no_argument, &flag, 10},
        {"no-push", no_argument, &flag, 11},
        {"max-concurrent-streams", required_argument, &flag, 12},
        {"expect-continue", no_argument, &flag, 13},
        {"encoder-header-table-size", required_argument, &flag, 14},
        {nullptr, 0, nullptr, 0}};
    int option_index = 0;
    int c =
        getopt_long(argc, argv, "M:Oab:c:d:gm:np:r:hH:vst:uw:W:", long_options,
                    &option_index);
    if (c == -1) {
      break;
    }
    switch (c) {
    case 'M':
      // peer-max-concurrent-streams option
      config.peer_max_concurrent_streams = strtoul(optarg, nullptr, 10);
      break;
    case 'O':
      config.remote_name = true;
      break;
    case 'h':
      print_help(std::cout);
      exit(EXIT_SUCCESS);
    case 'b':
      config.padding = strtol(optarg, nullptr, 10);
      break;
    case 'n':
      config.null_out = true;
      break;
    case 'p': {
      errno = 0;
      auto n = strtoul(optarg, nullptr, 10);
      if (errno == 0 && NGHTTP2_MIN_WEIGHT <= n && n <= NGHTTP2_MAX_WEIGHT) {
        config.weight.push_back(n);
      } else {
        std::cerr << "-p: specify the integer in the range ["
                  << NGHTTP2_MIN_WEIGHT << ", " << NGHTTP2_MAX_WEIGHT
                  << "], inclusive" << std::endl;
        exit(EXIT_FAILURE);
      }
      break;
    }
    case 'r':
#ifdef HAVE_JANSSON
      config.harfile = optarg;
#else  // !HAVE_JANSSON
      std::cerr << "[WARNING]: -r, --har option is ignored because\n"
                << "the binary was not compiled with libjansson." << std::endl;
#endif // !HAVE_JANSSON
      break;
    case 'v':
      ++config.verbose;
      break;
    case 't':
      config.timeout = util::parse_duration_with_unit(optarg);
      if (config.timeout == std::numeric_limits<double>::infinity()) {
        std::cerr << "-t: bad timeout value: " << optarg << std::endl;
        exit(EXIT_FAILURE);
      }
      break;
    case 'u':
      config.upgrade = true;
      break;
    case 'w':
    case 'W': {
      errno = 0;
      char *endptr = nullptr;
      unsigned long int n = strtoul(optarg, &endptr, 10);
      if (errno == 0 && *endptr == '\0' && n < 31) {
        if (c == 'w') {
          config.window_bits = n;
        } else {
          config.connection_window_bits = n;
        }
      } else {
        std::cerr << "-" << static_cast<char>(c)
                  << ": specify the integer in the range [0, 30], inclusive"
                  << std::endl;
        exit(EXIT_FAILURE);
      }
      break;
    }
    case 'H': {
      char *header = optarg;
      // Skip first possible ':' in the header name
      char *value = strchr(optarg + 1, ':');
      if (!value || (header[0] == ':' && header + 1 == value)) {
        std::cerr << "-H: invalid header: " << optarg << std::endl;
        exit(EXIT_FAILURE);
      }
      *value = 0;
      value++;
      while (isspace(*value)) {
        value++;
      }
      if (*value == 0) {
        // This could also be a valid case for suppressing a header
        // similar to curl
        std::cerr << "-H: invalid header - value missing: " << optarg
                  << std::endl;
        exit(EXIT_FAILURE);
      }
      config.headers.emplace_back(header, value, false);
      util::inp_strlower(config.headers.back().name);
      break;
    }
    case 'a':
#ifdef HAVE_LIBXML2
      config.get_assets = true;
#else  // !HAVE_LIBXML2
      std::cerr << "[WARNING]: -a, --get-assets option is ignored because\n"
                << "the binary was not compiled with libxml2." << std::endl;
#endif // !HAVE_LIBXML2
      break;
    case 's':
      config.stat = true;
      break;
    case 'd':
      config.datafile = optarg;
      break;
    case 'm':
      config.multiply = strtoul(optarg, nullptr, 10);
      break;
    case 'c': {
      auto n = util::parse_uint_with_unit(optarg);
      if (n == -1) {
        std::cerr << "-c: Bad option value: " << optarg << std::endl;
        exit(EXIT_FAILURE);
      }
      if (n > std::numeric_limits<uint32_t>::max()) {
        std::cerr << "-c: Value too large.  It should be less than or equal to "
                  << std::numeric_limits<uint32_t>::max() << std::endl;
        exit(EXIT_FAILURE);
      }
      config.header_table_size = n;
      config.min_header_table_size = std::min(config.min_header_table_size, n);
      break;
    }
    case '?':
      util::show_candidates(argv[optind - 1], long_options);
      exit(EXIT_FAILURE);
    case 0:
      switch (flag) {
      case 1:
        // cert option
        config.certfile = optarg;
        break;
      case 2:
        // key option
        config.keyfile = optarg;
        break;
      case 3:
        // color option
        color = true;
        break;
      case 4:
        // continuation option
        config.continuation = true;
        break;
      case 5:
        // version option
        print_version(std::cout);
        exit(EXIT_SUCCESS);
      case 6:
        // no-content-length option
        config.no_content_length = true;
        break;
      case 7:
        // no-dep option
        config.no_dep = true;
        break;
      case 9: {
        // trailer option
        auto header = optarg;
        auto value = strchr(optarg, ':');
        if (!value) {
          std::cerr << "--trailer: invalid header: " << optarg << std::endl;
          exit(EXIT_FAILURE);
        }
        *value = 0;
        value++;
        while (isspace(*value)) {
          value++;
        }
        if (*value == 0) {
          // This could also be a valid case for suppressing a header
          // similar to curl
          std::cerr << "--trailer: invalid header - value missing: " << optarg
                    << std::endl;
          exit(EXIT_FAILURE);
        }
        config.trailer.emplace_back(header, value, false);
        util::inp_strlower(config.trailer.back().name);
        break;
      }
      case 10:
        // hexdump option
        config.hexdump = true;
        break;
      case 11:
        // no-push option
        config.no_push = true;
        break;
      case 12:
        // max-concurrent-streams option
        config.max_concurrent_streams = strtoul(optarg, nullptr, 10);
        break;
      case 13:
        // expect-continue option
        config.expect_continue = true;
        break;
      case 14: {
        // encoder-header-table-size option
        auto n = util::parse_uint_with_unit(optarg);
        if (n == -1) {
          std::cerr << "--encoder-header-table-size: Bad option value: "
                    << optarg << std::endl;
          exit(EXIT_FAILURE);
        }
        if (n > std::numeric_limits<uint32_t>::max()) {
          std::cerr << "--encoder-header-table-size: Value too large.  It "
                       "should be less than or equal to "
                    << std::numeric_limits<uint32_t>::max() << std::endl;
          exit(EXIT_FAILURE);
        }
        config.encoder_header_table_size = n;
        break;
      }
      }
      break;
    default:
      break;
    }
  }

  int32_t weight_to_fill;
  if (config.weight.empty()) {
    weight_to_fill = NGHTTP2_DEFAULT_WEIGHT;
  } else {
    weight_to_fill = config.weight.back();
  }
  config.weight.insert(std::end(config.weight), argc - optind, weight_to_fill);

  set_color_output(color || isatty(fileno(stdout)));

  nghttp2_option_set_peer_max_concurrent_streams(
      config.http2_option, config.peer_max_concurrent_streams);

  if (config.encoder_header_table_size != -1) {
    nghttp2_option_set_max_deflate_dynamic_table_size(
        config.http2_option, config.encoder_header_table_size);
  }

  struct sigaction act {};
  act.sa_handler = SIG_IGN;
  sigaction(SIGPIPE, &act, nullptr);
  reset_timer();
  // cout<<"ok"<<endl;
  return run(argv + optind, argc - optind);
}

} // namespace nghttp2

int main(int argc, char **argv) {
  // Hung:
  string cmd = "bash ";
  cmd += meta.bwtrace;
  cmd += " &";
  cout << "================ cmd: " << cmd << endl;
  // if (system(cmd.c_str())) {
  //   std::cout << "could not run TC" << std::endl;
  // }
  // return 0;

  char filename[128];
  //
  sprintf(filename, "log_stream_ALG_%d.txt", meta.ALG);
  log_stream = fopen(filename, "w");
  fprintf(
      log_stream,
      "reqStreamId, "
      "pushStreamId,face_req_time,face_end_time,face_dow_thrp,face_late,face_"
      "cancel, iter_id,sid,fid,layerId, face_br,face_size,face_ini_weight\n");
  //
  sprintf(filename, "log_iter_ALG_%d.txt", meta.ALG);
  log_iter = fopen(filename, "w");
  fprintf(log_iter,
          "iter_id,iter_start_time,iter_end_time,iter_sam_thrp,iter_est_thrp,"
          "iter_base_buff,req_s_num,iter_down_time_budget\n");

  return nghttp2::run_app(nghttp2::main, argc, argv);
}
