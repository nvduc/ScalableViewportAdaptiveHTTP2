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

#include <openssl/err.h>
#include <math.h>

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
//#include <chrono>
//#include <thread>
//#include <mutex>
//#include <boost/algorithm/string.hpp>
//#include <boost/bind.hpp>
//#include <nghttp2/asio_http2_server.h>
#include <netdb.h>
#include <netinet/in.h>

#ifndef O_BINARY
#define O_BINARY (0)
#endif // O_BINARY
using namespace std;
//using namespace boost::posix_time;
int a=0;
std::vector<int> hung_rate_set = {200, 250, 300, 400, 500, 600, 700, 900, 1200, 1500, 2000, 2500, 3000, 4000, 5000, 6000};
int       hung_sd = 1000;


class Selection{
      public: 
        int bitrate;
        int numOfSegment;
        double requestCost, qualityCost, bufferCost, totalCost;
        Selection(){
          bitrate=0;
          numOfSegment=0;
        }
        Selection (int Bitrate,int NumOfSegment){
          bitrate=Bitrate;
          numOfSegment=NumOfSegment;
        }
  };

  int isHungModify = 1;
  int nextBitrate, nextN, curBitrate=0, tarBitrate=0;
  double margin = 0.96;
  int maxN = 4; 
  int MAX_NUMBER_OF_PUSH = 4;
  int pathLenth = 6;
  int curIndex = 0;
  int Bmin = 5000;
  std::vector <Selection> bestPath;
  std::vector <double> buffFlowBestPath;
  int num_of_case=hung_rate_set.size()*MAX_NUMBER_OF_PUSH;
  Selection *setPoint = new Selection[num_of_case];
  vector <vector<Selection>> trellis;
  long BUFFER_FLUCTUATION = hung_sd;
  int pushArray[] = {1000, 2000, 3000, 4000};

vector<vector<Selection>> buildTrellis(Selection *setPoint, int n) {
 // std::ios_base::sync_with_stdio(false);
    if (n == 1) {
      //cout<<"loop" <<endl;
      vector<vector<Selection>> trellis ;
      for (int i = 0; i < num_of_case; i++)
        if (true) {
             // cout<<"add";
         
          vector <Selection> path ;

          path.push_back(setPoint[i]);
          trellis.push_back(path);

        }
       //  cout<<"add"<<endl;
       // cout<<"finish";
      return trellis;
    } else {
      //cout<<"x"<<endl;
      vector<vector<Selection>> sub_trellis = buildTrellis(setPoint, n - 1);
      vector<vector<Selection>> tmp_trellis ;

      for (int i = 0; i < sub_trellis.size(); i++) {
        // if(i==sub_trellis.size()-1)
        //   cout<<sub_trellis.size()<<" "<<n<<endl;
        if(n==3) cout<<i<<endl;
        vector<Selection> path = sub_trellis.at(i);
//        double cur_aggre_bw = get_acc_bandwidth(path.toArray(new Selection[path.size()]));
        for (int j = 0; j < num_of_case; j++)       
        {
          vector<Selection> tmp_path ;
          tmp_path.reserve(path.size());
          copy(path.begin(),path.end(),back_inserter(tmp_path));
          // for(int k=0;k<path.size();k++){
          //   tmp_path.push_back(path.at(k));
          // }
          tmp_path.push_back(setPoint[j]);

          if (tmp_trellis.size() == 0)
            tmp_trellis.push_back(tmp_path);
          else {
            tmp_trellis.insert(tmp_trellis.begin(),tmp_path);
          }
        }
      }
      // System.out.println(tmp_trellis.size() + "");
      //cout<<"finish"<<endl;
      return tmp_trellis;
    }
  }

void init(){
  int count = 0;
    for (int j = hung_rate_set.size()-1; j >= 0; j--) {
    //for(int j = 0; j<manifest.NUMBER_OF_BITRATES;j++){
      for (int i = 0; i < MAX_NUMBER_OF_PUSH; i++){
        Selection temp = Selection(hung_rate_set.at(j),(int)(pushArray[i]/1000));
        setPoint[count++] = temp;
      }
    }
    //System.out.println("check1");
    //long t1 = System.currentTimeMillis();
    // ofstream f;
    // trellis = buildTrellis(setPoint, 3);
    // f.open("/home/haidang/Output/trellis.txt");
    // for(int i=0;i<trellis.size();i++){
    //   for(int j=0;j<trellis.at(i).size();j++){
    //     f<<trellis.at(i).at(j).bitrate<<" "<<trellis.at(i).at(j).numOfSegment<<" ";
    //   }
    //   f<<endl;
    // }
    // f.close();
    ifstream f;
    f.open("/home/haidang/Output/trellis.txt");
    int rate, N;
    while(!f.eof()){
        vector<Selection> temp;
        for(int i=0;i<3;i++){
          f >> rate;
          f >> N;
          temp.push_back(Selection(rate,N));
        }
        trellis.push_back(temp);
    }
    
   // cout<< trellis.size()<<endl;
    //cout<<" finish"<<endl;
}

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
    {3, 0, 201}, {5, 0, 101}, {7, 0, 1}, {9, 7, 1}, {11, 3, 1},
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
 //ms
int       hung_MAX_SEGMENTS = 2001;

// Hung: for clocks
int hung_sys_time = 0;
int hung_last_adapt_time[] = {0,0};
int hung_client_seg = 0;

// Hung: temporary store => don't understand why
std::string           hung_uri;
nghttp2_data_provider *hung_data_prd;
int64_t               hung_data_length;
nghttp2_priority_spec hung_pri_spec;
Headers               hung_headers;

// Hung: measurement
int              hung_tar_buff = 25000;
int              buff_Opt = 25000;
bool             hung_on_buffering = true;


double           hung_inst_thrp = 1000;
int              hung_cur_buff = 5000;
int dang_cur_buff[]={5500,5500};
int buff_max = buff_Opt;
double            thrp_est = 0, smoothedBW;

double               hung_safety_margin = 0.1;
bool first_request = true;
bool first_adapt =true;
enum Adaptation_method { AP, ATL, KPush };
enum Thrp_est_method {Instant, Mix};
//Thrp_est_method      thrp_est_method;
Adaptation_method    hung_method = KPush;
int                  hung_cur_rtt = 50;     // ik1 => 220ms
int                  hung_K = 4;
int num_of_request=1;

int first_choise =1;
// Hung: recorders
std::vector<int>     hung_seg_recorder;
std::vector<int>     hung_time_recorder;
std::vector<int>     hung_rate_recorder;
std::vector<double>  hung_thrp_recorder;
std::vector<int>     hung_buff_recorder;
vector<double> time_download_recorder;

std::vector<double> thrp_est_recorder,smooth_thrp_recoder;
//-------------------------------------------------------
double window_bits;
double safety_margin=0.1;
int num_of_segs =1;
int request_corr_id[10000];
// int block_stream;
// bool set_block=true;
int request1=0,request2=0;
nghttp2_data_provider *dang_data_prd;
int64_t               dang_data_length;
Headers               dang_headers;
std::string           dang_uri;
nghttp2_priority_spec dang_pri_spec;
double curr_thrp=0;
int dang_curr_request= 15;

const int num_of_tile=64;
bool need_sort=true;
int need_request=0;
//-------------------------------------------------------
vector<int> dang_rate_set_orginal = {109 ,113 ,251 ,257 ,470 ,653 ,877 ,1195 ,1771 ,2381 ,3152 ,4410 ,10085 ,10583 ,17926};
vector<int> dang_rate_set={20,30,60,80,110,150,220,300,390,550,1260,1320,2240};
int weight[num_of_tile]={-1,-1,-1,-1,-1,-1,-1,-1};
int choose_rate[num_of_tile]; 
double square[num_of_tile]={0,0,0,0,12,20,38,30};
 double sorted_square[num_of_tile];
  int tile_index[num_of_tile];
  vector<double> dang_thrp[num_of_tile];
  vector<double> dang_bitrate[num_of_tile];
  vector<int> dang_buffer;
// vector<int> rate_set1 = {100,300,400,600,900,1200,2000,3000};
// vector<int> rate_set2 = {200,300,500,600,900,1500,2500,3000,4000};
// vector<int> rate_set1 = {100,200, 250, 300, 400, 500, 600, 700, 900, 1200, 1500, 2000, 2500, 3000, 4000, 5000, 6000};
// vector<int> rate_set2 = {100,200, 250, 300, 400, 500, 600, 700, 900, 1200, 1500, 2000, 2500, 3000, 4000, 5000, 6000};
 vector<int> rate_set1 = {100,200,300, 500, 700, 900, 1200, 1500, 2000, 2500, 3000};
vector<int> rate_set2 = {100,200,300, 500, 700, 900, 1200, 1500, 2000, 2500, 3000};
int no_tile_of_view;
int init_rate=900;
int dang_adapted_rate[]={init_rate,init_rate};
bool change[]={false,false};
int chosen_rate[]={init_rate,init_rate};
int curr_bitrate[]={init_rate,init_rate};
vector<int> buffer1;
vector<int> buffer2;
int dang_buff_opt = 10000;
int dang_sd=1000;
int dang_last_segment=0;
int dang_last_time=0;
int dang_curr_buff=dang_buff_opt;
int dang_last_adapt_time=0;
double total_size_segment=0;
int dang_first_response=0;
int no_of_push_in_segment=0;
int rate_continuous=0;
int tile_in_segment=6;
int count_tile=0;
double dang_inst_thrp=0;
double dang_temp_thrp=0;
int start_measure_thrp;
int end_measure_thrp;
double total_data=0;
int total_time=0;
int hung_time_unit = 50;
ev_timer tev;
bool dang_connected=false;
bool dang_rst=false;
const int initial_segment = 1;
int dang_tile_buffer=(initial_segment-1)*64;
std::vector<int> dang_tile_buffer_record;
std::vector<double> dang_thrp_record;
const string first_tiles="20_21_28_29_36_37_44_45_3_4_5_6_11_12_13_14_19_22_27_30_35_38_43_46_51_52_53_54_59_60_61_62";
string rate_string;
bool canceled=false;
int curr_segment;
int seg_terminate=-1;
int no_request=0;
const int NO_REQUEST = 350;
int played = 1-initial_segment;
bool dang_buffering=false;
ofstream play_file;
int time_start_play = 0;
int time_start_buffering;
ofstream buffering_file;
ofstream tile_file;
bool decide_cancel=false;

double total_rate_request = 1787.75;
double total_rate_downloaded;
vector<double> dang_segment_rate;

int limited_dl_time = dang_sd;
//int num_of_tile=64;
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
void dang_terminate_segment(HttpClient *client);
void play_out(struct ev_loop *loop, ev_timer *w, int revents);
int dang_get_seg_from_uri (std::string uri) {
  int l = uri.find('_') + 1;
  int r = uri.find('_', l);
  return std::stoi(uri.substr(l, r-l));
}
int dang_get_video_from_uri (std::string uri) {
  return std::stoi(uri.substr(uri.rfind('_') + 1));
}

int dang_get_rate_from_uri (std::string uri) {
  int l;
  string sub = uri;
  for(int i=0;i<3;i++){
  l = sub.find('_') + 1;
 // cout<<l<<endl;
  //int r = uri.find('_', l);=
  sub = sub.substr(l);
  //cout<<sub<<endl;
}
  int r = sub.find("_");
  //cout<<sub.find("_")<<endl;
  return std::stoi(sub.substr(0, r));
  //return std::stoi(uri.substr(l, r-l));
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
  //cout<<"dcmm sao eo ra"<<endl;
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
  
    ev_timer_init(&tev,play_out,0,dang_sd*1.0/1000);
   // ev_timer_start(loop,&tev);
    play_file.open("/home/haidang/client/Output/play_file.txt");
    buffering_file.open("/home/haidang/client/Output/buffering.txt");
    tile_file.open("/home/haidang/client/Output/tile_file.txt");
    buffering_file << "Segment\tTile\tTime Buffering" << endl;
    tile_file << "Segment\tNum downloaded tile"<<endl;
    //cout<<"HttpClient"<<endl;
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
   //ev_timer_init(&tev, hung_tick_cb,0.,10.);
   // tev.repeat = hung_time_unit / 1000.0;
  //if(dang_connected)
   //ev_timer_again(loop,&tev);
  ev_io_set(&rev, fd, EV_READ);
  ev_io_set(&wev, fd, EV_WRITE);

  ev_io_start(loop, &wev);

  ev_timer_again(loop, &wt);

  return 0;
}

void HttpClient::disconnect() {
  state = ClientState::IDLE;
  play_file.close();
  for (auto req = std::begin(reqvec); req != std::end(reqvec); ++req) {
    if ((*req)->continue_timer) {
      (*req)->continue_timer->stop();
    }
  }

  ev_timer_stop(loop, &settings_timer);

  ev_timer_stop(loop, &rt);
  ev_timer_stop(loop, &wt);
  ev_timer_stop(loop,&tev);
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

  if (config.verbose) {
    print_timer();
    std::cout << " Connected" << std::endl;
    dang_connected=true;
  }

  state = ClientState::CONNECTED;

  ev_io_start(loop, &rev);
  ev_io_stop(loop, &wev);
ev_timer_again(loop,&tev);
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
  iv[niv].value = 1<<20;
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

namespace {
int on_frame_recv_callback2(nghttp2_session *session,
                            const nghttp2_frame *frame, void *user_data) {
  int rv = 0;

  if (config.verbose) {
    verbose_on_frame_recv_callback(session, frame, user_data);
  }

  auto client = get_client(user_data);
  switch (frame->hd.type) {
  case NGHTTP2_DATA: {
    auto req = static_cast<Request *>(
        nghttp2_session_get_stream_user_data(session, frame->hd.stream_id));
    if (!req) {
      return 0;
      ;
    }

    if (frame->hd.flags & NGHTTP2_FLAG_END_STREAM) {
      req->record_response_end_time();
      ++client->success;
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
    // if(dang_get_seg_from_uri(req->make_reqpath())==seg_terminate)
    //   decide_cancel=true;
    // else decide_cancel =false;
    switch (frame->headers.cat) {
    case NGHTTP2_HCAT_RESPONSE:
    case NGHTTP2_HCAT_PUSH_RESPONSE:
      check_response_header(session, req);
      break;
    case NGHTTP2_HCAT_HEADERS:
      if (req->expect_final_response) {
        check_response_header(session, req);
        break;
      }
      if ((frame->hd.flags & NGHTTP2_FLAG_END_STREAM) == 0) {
        nghttp2_submit_rst_stream(session, NGHTTP2_FLAG_NONE,
                                  frame->hd.stream_id, NGHTTP2_PROTOCOL_ERROR);
        cout<<"dcm0";
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
    //cout<<req->make_reqpath()<<endl;
    // if(decide_cancel){
    //   nghttp2_submit_rst_stream(session,NGHTTP2_FLAG_NONE,frame->push_promise.promised_stream_id,NGHTTP2_CANCEL);
    //   cout<<"Cancel for redundant tile"<<endl;
    //   //decide_cancel=false;
    // }
    //request_corr_id[frame->push_promise.promised_stream_id] = request_corr_id[frame->hd.stream_id];
    // Reset for response header field reception
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
      cout<<"dcm1";
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
      cout<<"dcm2";
      break;
    }
    req->uri = uri;
    req->u = u;

    if (client->path_cache.count(uri)) {
      nghttp2_submit_rst_stream(session, NGHTTP2_FLAG_NONE,
                                frame->push_promise.promised_stream_id,
                                NGHTTP2_CANCEL);
      cout<<"dcm3";
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
void Dang_adaptation_vr(HttpClient *client,int index);
void play_out(struct ev_loop *loop, ev_timer *w, int revents) {
  auto client = static_cast<HttpClient *>(ev_userdata(loop));
  int curr_time = std::chrono::duration_cast<std::chrono::milliseconds>(
                     get_time() - client->timing.connect_end_time).count();
    cout<<"["<<curr_time/1000.0 <<"]"<<"Inst Buffer : "<<dang_tile_buffer<<endl;
    // if(dang_tile_buffer<num_of_tile){
    //   dang_buffering=true;
    //   //dang_terminate_segment(client);
    //   seg_terminate=curr_segment;
    //  // Dang_adaptation_vr(client,2);
    // }
    if(dang_tile_buffer > 0){
      played++;
       if(dang_tile_buffer<num_of_tile){
          
          if(seg_terminate!=curr_segment){
        dang_terminate_segment(client);
        cout<<"Terminate 2"<<endl;
        seg_terminate=curr_segment;
        }
     // Dang_adaptation_vr(client,2);
      }
      cout<<"Seg playout "<<played<<endl;
    }
    else {
      dang_buffering=true;
      time_start_buffering = std::chrono::duration_cast<std::chrono::milliseconds>(
                     get_time() - client->timing.connect_end_time).count();
      ev_timer_stop(loop,&tev);
      
      cout<<"Cancel timer"<<endl;
      return;
    }
    if(dang_tile_buffer < num_of_tile){
      tile_file << curr_segment << '\t' <<dang_tile_buffer << endl;
    }
    if(dang_tile_buffer>num_of_tile)
      dang_tile_buffer-=num_of_tile;
    else dang_tile_buffer=0;

   
    

  //double waiting_intv = 50.0/1000;
 // w->repeat = waiting_intv;
 //  ev_timer_again(loop, w);
}
int getIndexByRate(int rate){
  for(int i=0;i<dang_rate_set_orginal.size();i++){
    if(rate==dang_rate_set_orginal.at(i))
      return i;
  }
  return -1;
}
// Hung: given the instant throughput, compute the maximum adapted bitrate
// In this method, the maximum adapted bitrate is considered as a rate candidate
//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
vector<double> callBuffFlowPath(vector<Selection> bestPath){
    vector<double> buffFlowPath ;
    int bitrate,N;
    double curBuf = hung_cur_buff;
    buffFlowPath.push_back(curBuf);
    for(int i = 0; i < bestPath.size(); i++) {
      bitrate = bestPath.at(i).bitrate;
      N = bestPath.at(i).numOfSegment;
      curBuf += (double)(hung_sd *N* (1 - bitrate / thrp_est));
      buffFlowPath.push_back(curBuf);
    }
    return buffFlowPath;  
  }


bool checkOrder(vector<Selection> path, int curBitrate){
    vector<Selection> tempPath = path ;
    // for(int i=0;i<path.size();i++){
    //   tempPath.push_back(path.at(i));
    // }
    Selection temp=Selection(curBitrate,1);
    tempPath.insert(tempPath.begin(), temp);
    /*System.out.println("check currentBR" + curBitrate);
    for(int i = 0; i<path.size();i++){
      System.out.println(path.get(i).bitrate);
    }*/
    int isIncrease = 0;
    //detect order
    for(int i = 1; i < tempPath.size();i++){      
      if(tempPath.at(i-1).bitrate-tempPath.at(i).bitrate<0) {       
        isIncrease = 1;
        break;
      }else if(tempPath.at(i-1).bitrate-tempPath.at(i).bitrate > 0){
        isIncrease = -1;
        break;
      }     
    }
    //check order
    if(isIncrease == 0)
      return true;
    for(int i = 1; i < tempPath.size();i++){
      if((tempPath.at(i-1).bitrate-tempPath.at(i).bitrate<0)&&(isIncrease == -1)) {       
        return false;
      }else if((tempPath.at(i-1).bitrate-tempPath.at(i).bitrate > 0)&&(isIncrease==1)){
        return false;
      }
      
    }
    return true;
  }
double getOverallCost(vector<Selection> path, int curBitrate, int tarBitrate) {
    double alpha = 10, beta = 13.5, gamma = 0.08;
//    double costRequest, costBuffer, costQuality;
    double averageN = 0, totalUsedBuffer = 0, maxChangeQ = 0;
    if (path.at(path.size()-1).bitrate != tarBitrate){
      //System.err.println("aaa");
      return 100000000;
    } else {
      totalUsedBuffer = hung_cur_rtt / 1000.0;
      for (int i = 0; i < path.size(); i++) {
        averageN += (double) path.at(i).numOfSegment / path.size();
        totalUsedBuffer += (double) path.at(i).numOfSegment * (hung_sd / 1000 )* (1 - (double) path.at(i).bitrate / thrp_est);
        int curVersion;
        if(i==0){
          curVersion = getIndexByRate(curBitrate);
        }else{
          curVersion = getIndexByRate(path.at(i - 1).bitrate);
        }
        int nextVersion = getIndexByRate(path.at(i).bitrate);
       // int abs_version = nextVersion - curVersion > 0? nextVersion - curVersion : curVersion-nextVersion;
        if (maxChangeQ < abs(nextVersion - curVersion) ){
          maxChangeQ = abs(nextVersion - curVersion);
        }
      }
    }
    /* must be higher than Bmin */
    if (hung_cur_buff - totalUsedBuffer * 1000 < Bmin)
      return 100000000;
    double deltaBuffer = (double) (buff_Opt- hung_cur_buff)/1000 - totalUsedBuffer;
    
    //double totalCost = beta * Math.exp(-alpha * averageN) + gamma * maxChangeQ + sigma * Math.exp(lamda*deltaBuffer);
//    double totalCost = (double)10*1/averageN + gamma * maxChangeQ + 0.08 * Math.exp(1*deltaBuffer);
    double totalCost = (double)alpha*1/averageN + beta * maxChangeQ + gamma * exp(1*deltaBuffer);
    return totalCost;

  }
  void getBestPath(int curBitrate,int tarBitrate){
    vector<Selection> path ;
    //trellis = buildTrellis(setPoint, 4);
    bestPath = trellis.at(0);
    
    double minCost = getOverallCost(bestPath,curBitrate,tarBitrate);
    for (int i = 1; i < trellis.size(); i++) {
      path = trellis.at(i);
      if(checkOrder(path,curBitrate)){
        double cost = getOverallCost(path,curBitrate,tarBitrate);
        if (cost < minCost) {
          bestPath = path;
          minCost = cost;         
        }
      }
    }
    //System.out.println(minCost);
  }

//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
//-------------------------------------------------------------------------

int hung_compute_max_adapted_rate (double thrp) {
  // compute rate candidate
  int rate_candidate = hung_rate_set.at(0); 
  for (auto rate = hung_rate_set.rbegin(); rate != hung_rate_set.rend(); ++rate) {
    //std::cout << std::endl << "[ADAPTATION] Take look " << *rate << " " << thrp << std::endl;
    if ((*rate) < thrp) { 
      rate_candidate = *rate;
      break; 
    }
  }
  return rate_candidate;
}

// Hung: rebufferring phase
void hung_req_vod_rebuff(HttpClient *client, bool submit = true) {
  // if(first_request){
  //     init();
  //     first_request=false;
  // }
  int num_of_segs = hung_tar_buff/hung_sd;
  if (hung_sys_time / hung_sd + num_of_segs > hung_MAX_SEGMENTS) {
    num_of_segs = hung_MAX_SEGMENTS - hung_sys_time / hung_sd - 1;
  }
  if (num_of_segs <= 0)
    return;
  
  std::string rate_string =  std::to_string(hung_rate_set.at(0));
  std::string num_string = std::to_string(num_of_segs);

  if (client->add_request(hung_uri+"/rebuff/bitrate="+rate_string+"/num="+num_string, 
                            hung_data_prd, hung_data_length, hung_pri_spec)) {
    if (submit) {
      if(hung_cur_buff > hung_tar_buff){
       //std::this_thread::sleep_for;
        //usleep((hung_cur_buff-hung_tar_buff) * 1000);
      }
      submit_request(client, hung_headers, client->reqvec.back().get());
    }
  }

  std::cout << "[REBUFF] bitrate " << rate_string << "\tnum " << num_string 
                            << std::endl << std::endl;  

  if (submit) client->signal_write();
}



// Hung: change the bitrate without refusing the stream
// Note that we disable adaptation until a new segment is fully received
void hung_req_vod_rate(HttpClient *client, int new_rate) {
  int num_of_segs = hung_K;
  if (hung_client_seg + num_of_segs > hung_MAX_SEGMENTS)
    num_of_segs = hung_MAX_SEGMENTS - hung_client_seg - 1;
  if (num_of_segs <= 0)
    return;

  std::string rate_string = std::to_string(new_rate);
  std::string num_string = std::to_string(num_of_segs);

  if (client->add_request(hung_uri+"/req_vod/bitrate="+rate_string+"/num="+num_string, 
                            hung_data_prd, hung_data_length, hung_pri_spec)) {
    if(hung_cur_buff > buff_max){
       //std::this_thread::sleep_for;
       // usleep((hung_cur_buff-buff_max) * 1000);
      }
    submit_request(client, hung_headers, client->reqvec.back().get());
    
  }

  std::cout << "[ADAPTATION] bitrate " << rate_string << "\tnum " << num_string 
                            << std::endl << std::endl;  
  num_of_request++;
  client->signal_write();
}
//-------------------------------------------------------------------------------
int max_rate_less_than_thrp(double thrp){
  //thrp = (1-safety_margin)*thrp;
  
    for(int i=dang_rate_set_orginal.size()-1;i>=0;i--){
      if(dang_rate_set_orginal.at(i)<=thrp)
        return dang_rate_set_orginal.at(i);
    }
    return dang_rate_set_orginal.at(0); 
}
int *max_rate_less_than_thrp2(double thrp){
  //cout<<rate_set1.size()<<endl<<rate_set2.size()<<endl;
  thrp = (1-safety_margin)*thrp;
  vector<int*> rate;
  
  int max_rate = 0;
  bool check[2];
  check[0]=true;
  check[1]=true;
  for(int i=rate_set1.size()-1;i>=0;i--){
    if(!check[0]) break;
    else
      if(rate_set1.at(i)<=thrp){
        for(int j=rate_set1.size()-1;j>=0;j--)
          if(rate_set1.at(i)+rate_set2.at(j)<=thrp){
            max_rate = rate_set1.at(i)+rate_set2.at(j);
          
            
            check[0]=false;
            break;
          }
      }
  }

  for(int i=rate_set2.size()-1;i>=0;i--){
    if(!check[1]) break;
    else
      if(rate_set2.at(i)<=thrp){
        for(int j=rate_set1.size()-1;j>=0;j--)
          if(rate_set2.at(i)+rate_set1.at(j)<=thrp && rate_set2.at(i)+rate_set1.at(j)>max_rate){
            max_rate = rate_set2.at(i)+rate_set1.at(j);

        
              
              check[1]=false;
          }
      }
    }
    if(max_rate==0) max_rate = rate_set1.at(0)+rate_set2.at(0);
    //cout<<rt[0]<<endl<<rt[1]<<endl;
    //rate.push_back(rt);
   // cout<<max_rate<<endl;
    for(int i=rate_set1.size()-1;i>=0;i--){
      if(rate_set1.at(i)<max_rate){
        for(int j=rate_set2.size()-1;j>=0;j--)
          if(rate_set1.at(i)+rate_set2.at(j)==max_rate){
            int *rt = new int [2];
              rt[0] = rate_set1.at(i);
              rt[1] = rate_set2.at(j);
              //cout<<rt[0]<<'\t'<<rt[1]<<endl;
              rate.push_back(rt);
             
          }
      }
    }
    // for(int i=0;i<rate.size();i++){
    //   cout<<rate.at(i)[0]<<'\t'<<rate.at(i)[1]<<endl;
    // }
    //cout<<rate.at(0)[0]<<endl;
    int *final_rate = new int [2];
    int min_distance = 10000000;
    for(int i=0;i<rate.size();i++){
      int *RT = new int [2] ;
      RT = rate.at(i);
      if(abs(RT[0]-RT[1])< min_distance){
        min_distance=abs(RT[0]-RT[1]);
        final_rate = RT;
      }
    }
    return final_rate;
}

int gcd(int a, int b) {
    return b == 0 ? a : gcd(b, a % b);
}
int *find_weight(int a, int b){
    int *weight =new int [2];
    int c= gcd(a,b);
    weight[0]=a/c;
    weight[1]=b/c;
    return weight;
}
void Find_weight(){
  int temp;
  for(int i=0;i<num_of_tile;i++){
    temp=getIndexByRate(choose_rate[i]);
    weight[i]=dang_rate_set[temp]/10;
  }
}
void dang_req_vod_rate(HttpClient *client, int rate1, int rate2 ,int tile,bool submit=true) {
  

    //std::string rate_string = std::to_string(new_rate);
    std::string num_string = std::to_string(num_of_tile);
    rate_string = "";
    for(int i = 0;i<8;i++)
      rate_string += (to_string(rate1) + "_");
    for(int i = 0;i<24;i++){
      rate_string += to_string(rate2);
      if(i < 23)
        rate_string += "_";
    }

    if (client->add_request(dang_uri+"/rebuff"+to_string(tile)+"/bitrate="+rate_string+"/num="+num_string+"/"+first_tiles, 
                              dang_data_prd, dang_data_length, dang_pri_spec)) {
      
    
   if(submit)
      submit_request(client, dang_headers, client->reqvec.back().get()); 
    }
 
  dang_curr_request+=2;
  //if(mode==1)
    //request_corr_id[dang_curr_request]=tile;
    std::cout << "[ADAPTATION] bitrate for video"<< to_string(tile)<<" "<< rate_string << "\tnum " << num_string 
                              << std::endl << std::endl;  
    
    client->signal_write();
  
}
void dang_terminate_segment(HttpClient *client){
  if(no_request>NO_REQUEST-2)
    return;
  if(client->add_request(dang_uri+"/terminate_segment",dang_data_prd,dang_data_length,dang_pri_spec));
    submit_request(client,dang_headers,client->reqvec.back().get());
    cout<<"Terminate segment"<<endl;
}

void dang_change_rate(HttpClient *client, int new_rate, int video) {
  if(dang_adapted_rate [video-1]== new_rate) return;
  else dang_adapted_rate[video-1] = new_rate;

  // if(curr_bitrate[video-1]==chosen_rate[video-1]) change[video-1]=false;
  // else change[video-1]=true;

  // if(curr_bitrate[video-1]!=chosen_rate[video-1] && change[video-1]) return;

  
  // chosen_rate[video-1]=new_rate;

  std::string rate_string = std::to_string(new_rate);
 // std::string num_string = std::to_string(num_of_segs);
  // int old_rate=hung_rate_recorder.at(hung_rate_recorder.size()-1);
  // if(new_rate < old_rate){
  //     num_of_switch_down ++;
  //     int old_version = getIndexByRate(old_rate);
  //     int new_version = getIndexByRate(new_rate);
  //     if(old_version-new_version > max_step)
  //       max_step=old_version-new_version;
  //     if(old_version-new_version>=3) switch_down_greater_3 ++;
  //   }
  //   num_of_request++;
  if (client->add_request(dang_uri+"/change"+to_string(video)+"/"+rate_string, dang_data_prd, dang_data_length, dang_pri_spec)) {
    submit_request(client, dang_headers, client->reqvec.back().get());
  }

  std::cout << "[ADAPTATION] bitrate " << rate_string <<" of tile "<<video
                            << std::endl << std::endl;  

  client->signal_write();
}
void selection_sort()
{ 
  for(int i=0;i<num_of_tile;i++){
    sorted_square[i]=square[i];
    tile_index[i]=i+1;

  }
    double temp_square;
    int temp_index;
    for (int i=0;i<num_of_tile;i++)
        for(int j=0;j<i;j++)
           {
                if(sorted_square[i]<sorted_square[j])
                {
                temp_square=sorted_square[i];
                sorted_square[i]=sorted_square[j];
                sorted_square[j]=temp_square;
                temp_index=tile_index[i];
                tile_index[i]=tile_index[j];
                tile_index[j]=temp_index;
                }
           }
}
// void Dang_algorithm_priority(HttpClient *client){
//   cout<<endl<<"Thrp : "<<hung_inst_thrp<<endl;
//   //int *rate = max_rate_less_than_thrp2(hung_inst_thrp);

//   // int rate1,rate2;
//   // rate1=max_rate_less_than_thrp(hung_inst_thrp*1.0/2,1);
//   // rate2=max_rate_less_than_thrp(hung_inst_thrp-rate1*1.0/(1-safety_margin),2);
//   // int rate3,rate4;
//   // rate4=max_rate_less_than_thrp(hung_inst_thrp*1.0/2,2);
//   // rate3=max_rate_less_than_thrp(hung_inst_thrp-rate4*1.0/(1-safety_margin),1);

//   // // if(first_choise==1){
//   // //   rate1=rate3;
//   // //   rate2=rate4;
//   // //   first_choise = 2;
//   // // }
//   // // else first_choise=1;
//   // if(rate1+rate2 < rate3+rate4){
//   //   rate1=rate3;
//   //   rate2=rate4;
//   // }
//   if(need_sort){
//     selection_sort();
//     no_tile_of_view=0;
//     for(int i=0;i<num_of_tile;i++)
//       if(square[i]>0)
//         no_tile_of_view++;
//     need_sort=false;
//   }
//   //if(dang_curr_buff>dang_buff_opt)
//     //usleep((dang_curr_buff-dang_buff_opt)*1000);
//  double thrp_temp=hung_inst_thrp;
//   for(int i=0;i<num_of_tile;i++){
//     if(sorted_square[i]==0)
//     //cout<<thrp_temp*sorted_square[i]*(1-safety_margin)/100<<endl;
//       choose_rate[i]=max_rate_less_than_thrp(thrp_temp*sorted_square[i]*(1-safety_margin)*1.0/100);
//     else {
//       int total_square=0;
//       for(int j=i;j<num_of_tile;j++)
//           total_square+=sorted_square[j];
//       choose_rate[i]=max_rate_less_than_thrp(thrp_temp*sorted_square[i]*(1-safety_margin)*1.0/total_square);
//     }
//     thrp_temp-=choose_rate[i];
//   }
//   // for(int i=0;i<num_of_tile;i++)
//   //   cout<<sorted_square[i]<<'\t'<<endl;
//   // for(int i=0;i<num_of_tile;i++)
//   //   cout<<tile_index[i]<<'\t'<<endl;
//   Find_weight();
//   //for(int i=0;i<num_of_tile;i++){
//     //nghttp2_priority_spec_init(&dang_pri_spec, 11, weight[i], 0);
//     //int temp_rate=getIndexByRate(choose_rate[i]);
//   if(tile_in_segment > 3)
//     rate_continuous=max_rate_less_than_thrp(sorted_square[tile_in_segment] * hung_inst_thrp*(1-safety_margin)/100);
//   else rate_continuous=dang_rate_set_orginal.at(0);
//     dang_req_vod_rate(client,rate_continuous,tile_index[tile_in_segment]);
//   //}
//   // int *rate=max_rate_less_than_thrp2(hung_inst_thrp);
//   // int *dang_weight = find_weight(rate[0],rate[1]);
//   // nghttp2_priority_spec_init(&dang_pri_spec, 11, dang_weight[0], 0);
//   // dang_req_vod_rate(client,rate[0],1);
//   // nghttp2_priority_spec_init(&dang_pri_spec, 11, dang_weight[1], 0);
//   // dang_req_vod_rate(client,rate[1],2);
// }
void Dang_adaptation_vr(HttpClient *client,int index){
    if(no_request > NO_REQUEST-2)
      return;
    no_request++;
    double avail_thrp = dang_inst_thrp*(1-safety_margin)*1.0;
    if(total_time > dang_sd) {
      limited_dl_time = 2*dang_sd - total_time;
      avail_thrp = avail_thrp * limited_dl_time *1.0/dang_sd;
    }
    else limited_dl_time = dang_sd;
    int rate1 = 0, rate2 = 0;
      // if(dang_tile_buffer >= num_of_tile*(initial_segment)){
        if(dang_tile_buffer >= 8 || played < curr_segment){
        int curr_time = std::chrono::duration_cast<std::chrono::milliseconds>(
                     get_time() - client->timing.connect_end_time).count();
        cout<<"["<<curr_time/1000.0<<"]";
        curr_time -= time_start_play;
        int sleep_time = dang_sd*(int)(curr_time/dang_sd+1) - curr_time;
        if(sleep_time<1000){
          cout<<"Sleeping prevent over flow : "<<sleep_time<<" start time " <<time_start_play<<endl;
          usleep(sleep_time*1000);
        }
       }
      //curr_segment++;
       if(!dang_buffering){
        for(int i = dang_rate_set_orginal.size() - 1 ; i >= 1 ; i --){
          if(8.0*dang_rate_set_orginal.at(i)/num_of_tile + 24.0*dang_rate_set_orginal.at(i-1)/num_of_tile 
            + 32.0 * dang_rate_set_orginal.at(0)/num_of_tile <= avail_thrp){
            rate1 = dang_rate_set_orginal.at(i);
            rate2 = dang_rate_set_orginal.at(i-1);
            break;
          }
        }
        if(rate2 == 0 && rate1 == 0){
          rate1 = dang_rate_set_orginal.at(0);
          rate2 = dang_rate_set_orginal.at(0);
        }
      }
      else {
        rate1 = dang_rate_set_orginal.at(0);
        rate2 = dang_rate_set_orginal.at(0);
      }
     // int new_rate=max_rate_less_than_thrp(dang_inst_thrp*(1-safety_margin)*1.0);
      total_rate_request = 8.0*rate1/num_of_tile + 24.0*rate2/num_of_tile 
          + 32.0 * dang_rate_set_orginal.at(0)/num_of_tile;
      dang_req_vod_rate(client,rate1,rate2,index);
      cout<<"Adaptation bitrate = "<<rate1<<" "<<rate2<<" Thrp "<<dang_inst_thrp<<endl;
}
// void Dang_algorithm_priority_surveillance(HttpClient *client){
//   //int *rate = max_rate_less_than_thrp2(hung_inst_thrp);

//   int rate1,rate2;
//   // rate1=max_rate_less_than_thrp(hung_inst_thrp*0.7,1);
//   // rate2=max_rate_less_than_thrp(hung_inst_thrp-rate1*1.0/(1-safety_margin),2);
//   int rate3,rate4;
//   rate4=max_rate_less_than_thrp(hung_inst_thrp*0.3,2);
//   rate3=max_rate_less_than_thrp(hung_inst_thrp-rate4*1.0/(1-safety_margin),1);
//   // int avail_buff = buffer2.at(buffer2.size()-1) - dang_buff_opt - hung_cur_rtt;
//   // rate1=max_rate_less_than_thrp(hung_inst_thrp*0.7,1);
//   // int max_rate = max_rate_less_than_thrp (avail_buff * hung_inst_thrp*0.3 / hung_sd,2);
//   // rate2 = max_rate_less_than_thrp(hung_inst_thrp*0.3,2);
//   // if(max_rate > rate2)
//   //   rate2=max_rate;
//   // if(first_choise==1){
//   //   rate1=rate3;
//   //   rate2=rate4;
//   //   first_choise = 2;
//   // }
//   // else first_choise=1;
//   //if(rate1+rate2 < rate3+rate4){
//     rate1=rate3;
//     rate2=rate4;
//   //}
//   int *dang_weight = find_weight(rate1,rate2);
//   cout<<rate1<<'\t'<<rate2<<endl;
//   nghttp2_priority_spec_init(&dang_pri_spec, 11, dang_weight[0], 0);
//   nghttp2_submit_priority(client->session, NGHTTP2_FLAG_NONE, 13, &dang_pri_spec);
//  dang_change_rate(client,rate1,1);
//   nghttp2_priority_spec_init(&dang_pri_spec, 11, dang_weight[1], 0);
//   nghttp2_submit_priority(client->session, NGHTTP2_FLAG_NONE, 15, &dang_pri_spec);
//  dang_change_rate(client,rate2,2);
// }

// void Dang_algorithm_priority_surveillance2(HttpClient *client,int video){
//   //int *rate = max_rate_less_than_thrp2(hung_inst_thrp);

//   double thrp ;
//   int dang_new_rate;
//   if(video==1) {
//     thrp = dang_thrp1.at(dang_thrp1.size()-1);
//     dang_new_rate = max_rate_less_than_thrp(thrp,1);
//     dang_change_rate(client,dang_new_rate,1);
//   }
//   else {
//     thrp = dang_thrp2.at(dang_thrp2.size()-1);
//     // int avail_buff = buffer2.at(buffer2.size()-1) - dang_buff_opt - hung_cur_rtt;
//     // int max_rate = max_rate_less_than_thrp (avail_buff * thrp / hung_sd,2);
//     // dang_new_rate = max_rate_less_than_thrp(thrp,2);
//     // if(max_rate>dang_new_rate)
//     //   dang_new_rate=max_rate;
//     dang_new_rate = max_rate_less_than_thrp(thrp,2);
//     //dang_change_rate(client,dang_new_rate,1);
//      dang_change_rate(client,dang_new_rate,2);
//   }
//  //  if
//  //  //int *dang_weight = find_weight(rate1,rate2);
//  //  //cout<<rate1<<'\t'<<rate2<<endl;
//  //  nghttp2_priority_spec_init(&dang_pri_spec, 11, dang_weight[0], 0);
//  //  nghttp2_submit_priority(client->session, NGHTTP2_FLAG_NONE, 13, &dang_pri_spec);
//  // // dang_change_rate(client,rate1,1);
//  //  nghttp2_priority_spec_init(&dang_pri_spec, 11, dang_weight[1], 0);
//  //  nghttp2_submit_priority(client->session, NGHTTP2_FLAG_NONE, 15, &dang_pri_spec);
//  // dang_change_rate(client,rate2,2);
// }


//---------------------------------------------------------------------------------------------
// Hung: KPush method. Note that the way to compute the throughput is totally different segment thrp
// So, the Push-1 is different from HTTP/1.1-based method
// void hung_KPush_method (HttpClient *client) {
//   std::cout << std::endl << "[ADAPTATION] Invoke at time " << hung_sys_time << "ms \t Thrp: " 
//                            << (int)hung_inst_thrp << "kbps \t Buff: "<< hung_cur_buff << "ms" << std::endl;

//   if (hung_cur_buff <= hung_sd) {
//     hung_on_buffering = true;
//     hung_cur_buff = 0;
//     hung_req_vod_rebuff(client);
//     return;
//   } 

//   if (hung_on_buffering && hung_cur_buff == hung_tar_buff) {
//     hung_on_buffering = false;
//   }
//   int rate_candidate;
//   if(first_adapt) {
//     rate_candidate=2500;
//     first_adapt=false;
//   }
//   else {

//     int cur_index = hung_thrp_recorder.size()-1;
//     double sum = 0; int count = 0;
//     for (int i = cur_index; i > cur_index - 3; i--) 
//       if (i >= 0) { 
//         sum += hung_thrp_recorder.at(i); 
//         count++;
//       }
//     double avg_thrp = sum / count;

//     double avg_thrp_with_RTT = avg_thrp * (hung_K * hung_sd - hung_cur_rtt) / (hung_K * hung_sd);
//     // cannot use the segment throughput until we compute a rtt

//     double avail_thrp = (1 - hung_safety_margin) * hung_inst_thrp; 
//     rate_candidate = hung_compute_max_adapted_rate (avail_thrp);
//  }
//   hung_req_vod_rate(client, rate_candidate);
// }
// // dang  --------------------------------
// //--------------------------------------
// void setSmoothedBW(){
//    double sigma = 0.125;
//           if (hung_thrp_recorder.size() <= 1){
//               smoothedBW = hung_inst_thrp;
//           }else{
//               //int curr_rate = hung_rate_recorder.at(hung_rate_recorder.size()-1);
//             //double bw =hung_thrp_recorder.at(hung_thrp_recorder.size()-1); 
//             smoothedBW = hung_inst_thrp * sigma + smoothedBW * (1 - sigma);
//             }
// }
// void set_estimate_thrp(Thrp_est_method mode){
//       if(mode == Instant){
//         thrp_est = hung_inst_thrp;
//         return;
//       }
//       if(mode == Mix){
         
//         thrp_est = smoothedBW < hung_inst_thrp ? smoothedBW : hung_inst_thrp;
//         return;
//     }
// }

// void Seamless_method (HttpClient *client){
//       std::cout << std::endl << "[ADAPTATION] Invoke at time " << hung_sys_time << "ms \t Thrp: " 
//                            << (int)hung_inst_thrp << "kbps \t Buff: "<< hung_cur_buff << "ms" << std::endl;

//       if (hung_cur_buff <= hung_sd) {
//         hung_on_buffering = true;
//         hung_cur_buff = 0;
//         hung_req_vod_rebuff(client);
//         return;
//       } 

//       if (hung_on_buffering && hung_cur_buff == hung_tar_buff) {
//         hung_on_buffering = false;
//       }
//       int next_rate;
//       if(first_adapt){
//         first_adapt=false;
//         next_rate = hung_compute_max_adapted_rate(hung_inst_thrp*(1-hung_safety_margin));
//        //next_rate=2500;
//         hung_req_vod_rate(client, next_rate);
//         return;
//       }



//       curBitrate = hung_rate_recorder.at(hung_rate_recorder.size()-1);
//       Thrp_est_method thrp_est_method;
//     if (hung_inst_thrp < curBitrate) {
//       // estimate the throughput
//       thrp_est_method = Instant;
//       set_estimate_thrp(thrp_est_method);
      
//       // search to find out the new path for the next throughput change.
//       if (bestPath.size()==0|| abs(hung_cur_buff - buffFlowBestPath.at(curIndex)) > BUFFER_FLUCTUATION) {
//         tarBitrate = hung_compute_max_adapted_rate(thrp_est*(1-hung_safety_margin));
// //        long t1 = System.currentTimeMillis();
//         getBestPath(curBitrate, tarBitrate);
// //        System.out.println("################################################################################################################ " + (System.currentTimeMillis()-t1));
//         buffFlowBestPath = callBuffFlowPath(bestPath);
// //        System.out.println(bestPath.toString());
// //        System.out.println(buffFlowBestPath.toString());
//         curIndex = 0; 
//       }
  
//       // if don't complete a way, just select the next bitrate
//       if (bestPath.size() > 0 && curIndex < bestPath.size()) { 
//         //System.out.println("Current Index: " + curIndex);
//         nextBitrate = bestPath.at(curIndex).bitrate;
//         nextN = bestPath.at(curIndex).numOfSegment;
//         curIndex++;
//       // if complete a way, build path
//       } else {
//         tarBitrate = hung_compute_max_adapted_rate(hung_inst_thrp*(1-hung_safety_margin));
//         getBestPath(curBitrate, tarBitrate);
//         buffFlowBestPath = callBuffFlowPath(bestPath);
//         curIndex = 0; 
//         nextBitrate = bestPath.at(curIndex).bitrate;
//         nextN = bestPath.at(curIndex).numOfSegment;
//         curIndex++;
//         cout<<"beginBitrate " << curBitrate << " Target " << tarBitrate<<endl;
//       }
      
//       // print path
//       for (int i = 0; i < bestPath.size(); i++) {
//         cout<<bestPath.at(i).bitrate<<" " ;             
//       }
//       cout<<endl;
//       for (int i = 0; i < bestPath.size(); i++) {
//         cout<<bestPath.at(i).numOfSegment<<" " ;              
//       }
//       cout<<endl;
//       cout<<"Cur Bitrate " << curBitrate << " current Index " << curIndex;
//       cout<<endl;
      
//     }else{
//       // estimate the throughput
//       thrp_est_method = Mix;
//       set_estimate_thrp(thrp_est_method);
//       //setEstimatedNextThroughput(ESTIMATE_BW_MIXED);
      
//       bestPath.clear();
//       if (hung_cur_buff < hung_tar_buff){
//         nextBitrate = curBitrate;
//         if(isHungModify == 1){
//           nextN = 1;
//           double tempBuf = hung_cur_buff; 
//           while(tempBuf < hung_tar_buff && nextN < maxN){
//             nextN ++;
//             tempBuf = hung_cur_buff + nextN*hung_sd*(1-(double)curBitrate/thrp_est);
//           }
//         }
//       } else {
//         nextBitrate = hung_compute_max_adapted_rate(thrp_est*(1-hung_safety_margin));
// //        System.out.println("################################  " + nextBitrate);
//         if(isHungModify == 1){
//           nextN = maxN;
//         }

//       }
//     }
//     hung_K=nextN;
//     hung_req_vod_rate(client,nextBitrate);
// //      if(isHungModify == 0){
// //        nextN = (int) (pushArray[maxN-1]/Manifest.REAL_SEGMENT_DURATION);
// //        while (nextN > 1){
// //          if((buffer.getBufferSize() - (float)nextN*Manifest.REAL_SEGMENT_DURATION*nextBitrate/estimatedThroughput/alpha) >= Bmin){
// //            break;
// //          }
// //          nextN--;
// //        }
// //      }
              
    
    


// }
//-----------------------------------------------
//----------------------------------------------

namespace {
int on_stream_close_callback(nghttp2_session *session, int32_t stream_id,
                             uint32_t error_code, void *user_data) {
  auto client = get_client(user_data);
  auto req = static_cast<Request *>(
      nghttp2_session_get_stream_user_data(session, stream_id));

  if (!req) {
    return 0;
  }
  // if(request_corr_id[stream_id]==1)
  //     request1++;
  //   else request2++;
    //cout<<"video index "<<dang_get_video_from_uri(req->make_reqpath())<<endl;
  // If this request is using Expect/Continue, stop its ContinueTimer.
  if (req->continue_timer) {
    req->continue_timer->stop();
  }

  update_html_parser(client, req, nullptr, 0, 1);
  ++client->complete;

      // Hung: record the results
      if (req->stream_id % 2 == 0) {
        cout<<"real len "<<req->response_len<<endl;
      //
        if(dang_get_video_from_uri(req->make_reqpath())==1)
      request1++;
    else request2++;
        // num_of_request++;

        hung_sys_time = std::chrono::duration_cast<std::chrono::milliseconds>(
                     get_time() - client->timing.connect_end_time).count();

        long download_intv_us = std::chrono::duration_cast<std::chrono::microseconds>(
                     req->timing.response_end_time - req->timing.response_start_time).count();
        std::cout << "[INFO] measuared throughput in " << (download_intv_us / 1000) << "ms";
        auto temp_thrp = (double)req->response_len * 8 * 1000 / download_intv_us;
        if (download_intv_us > 500 || temp_thrp < 1500)
          hung_inst_thrp = temp_thrp;
        else 
          hung_inst_thrp = 1500;
       // if(hung_inst_thrp>5000) hung_inst_thrp=hung_thrp_recorder.at(hung_rate_recorder.size()-1);
        std::cout << "\t New Instant thrp: " << hung_inst_thrp << std::endl;

        // Compute RTT but be careful because of live streaming. RTT has to be computed by PING
        // ...
        //  if(dang_get_video_from_uri(req->make_reqpath())==1)
        //       dang_thrp1.push_back(hung_inst_thrp);
        // else dang_thrp2.push_back(hung_inst_thrp);
        if(req->response_len!=0){
          count_tile++;
            //cout<<"test "<<dang_get_seg_from_uri(req->make_reqpath())<<'\t'<<played<<'\t';
          if(dang_get_seg_from_uri(req->make_reqpath())!=played)
              dang_tile_buffer++;
          cout<<dang_tile_buffer<<endl;
          dang_tile_buffer_record.push_back(dang_tile_buffer);
          if(count_tile==1){
            start_measure_thrp = std::chrono::duration_cast<std::chrono::milliseconds>(
                      req->timing.response_start_time - client->timing.connect_end_time).count();
          
           
          }
          total_data+=req->response_len * 8.0/1000;
          
          cout<<"Count Tile : "<<count_tile<<endl;
          total_time = std::chrono::duration_cast<std::chrono::milliseconds>(
                    req->timing.response_end_time - client->timing.connect_end_time).count() - start_measure_thrp;
          cout<<"Total data "<<total_data<<"\tTotal time :" << total_time<<endl;
          dang_temp_thrp = total_data*1000.0/total_time;
        // else if(count_tile==num_of_tile||canceled){
        //   count_tile=0;
        //   if(canceled)
        //     canceled=false;
         
        //   end_measure_thrp = std::chrono::duration_cast<std::chrono::milliseconds>(
        //             req->timing.response_end_time - client->timing.connect_end_time).count();
        //   dang_inst_thrp = total_data*1000.0/(end_measure_thrp-start_measure_thrp);
        //   cout<<"Total data "<<total_data<<"\tTotal time" << end_measure_thrp-start_measure_thrp<<endl;
        //    total_data=0;
           
        // }

    
        //cout<<co
        

        int tile=dang_get_video_from_uri(req->make_reqpath());
        
       // if(tile>=1&&tile<=4) hung_inst_thrp=hung_thrp_recorder.at(hung_rate_recorder.size()-1);
        curr_segment=dang_get_seg_from_uri(req->make_reqpath());
        // if(tile==32&&curr_segment==1) {
        //          dang_rst=true;
                 

        // }
        //dang_thrp[tile-1].push_back(hung_inst_thrp);
        //hung_client_seg = dang_get_seg_from_uri(req->make_reqpath());

        // hung_seg_recorder.push_back(hung_client_seg);
        // hung_time_recorder.push_back(hung_sys_time);
        // hung_thrp_recorder.push_back(hung_inst_thrp);
        // time_download_recorder.push_back(download_intv_us/1000);
        //--------------------
        //--------------------
        int rate = dang_get_rate_from_uri(req->make_reqpath());
       // hung_rate_recorder.push_back(rate);

        // if (hung_on_buffering)
        //     hung_cur_buff += hung_sd;
        // else
        int video=dang_get_video_from_uri(req->make_reqpath());
        int time_remaining = (total_rate_request *dang_sd/1000.0 - total_data)*1000/(dang_temp_thrp*(1 - safety_margin));
        cout<<"Time remaining "<<time_remaining<<endl;
          if(dang_tile_buffer >= 8 && total_time + time_remaining > limited_dl_time/0.9 && seg_terminate != curr_segment){
            dang_terminate_segment(client);
            cout<<"Terminate 1"<<endl;
            seg_terminate = curr_segment;
          }
          dang_thrp_record.push_back(dang_inst_thrp);
        hung_cur_buff += hung_sd - (hung_sys_time - hung_last_adapt_time[video-1]);

        //dang_cur_buff[video-1] += hung_sd - (hung_sys_time - hung_last_adapt_time[video-1]);
        
       //hung_buff_recorder.push_back(hung_cur_buff);
        no_of_push_in_segment++;
        total_size_segment+=(double) req->response_len*8/1000;
        if(curr_segment!=dang_last_segment){
            dang_first_response=hung_sys_time-(int)download_intv_us/1000;
            dang_curr_buff+=dang_sd;
            dang_curr_buff-=hung_sys_time-dang_last_adapt_time;
            dang_last_segment=curr_segment;
        }
         else dang_curr_buff-=hung_sys_time-dang_last_time;
        if(no_of_push_in_segment==num_of_tile){
          long total_time = hung_sys_time-dang_first_response;
          cout<<total_size_segment<<'\t'<<total_time<<endl;
        //  hung_inst_thrp = total_size_segment*1000.0/total_time;
          no_of_push_in_segment=0;
          total_size_segment=0;
        }
         cout<<"OK"<<endl;
    //   nghttp2_submit_rst_stream(session, NGHTTP2_FLAG_NONE, 10000, NGHTTP2_CANCEL);

        // dang_last_time=hung_sys_time;
        // dang_buffer.push_back(dang_curr_buff);
        // if(video==1) buffer1.push_back(dang_cur_buff[0]);
        // else buffer2.push_back(dang_cur_buff[1]);
        // hung_last_adapt_time[video-1] = hung_sys_time;
        // //setSmoothedBW();
        // thrp_est_recorder.push_back(thrp_est);
        // smooth_thrp_recoder.push_back(smoothedBW);

        
         // int index = dang_thrp1.size()-1;

          if(dang_tile_buffer == 64 && dang_buffering){
            dang_buffering=false;
            time_start_play = std::chrono::duration_cast<std::chrono::milliseconds>(
                     get_time() - client->timing.connect_end_time).count();
            buffering_file << dang_get_seg_from_uri(req->make_reqpath())<<'\t'
                    <<dang_get_video_from_uri(req->make_reqpath())<<'\t'
                    << (time_start_play-time_start_buffering)/1000.0
                    <<" from "<<time_start_buffering<<"s to "
                    <<time_start_play<<"s" <<endl;

            // dang_terminate_segment(client);
            // terminate_segment=dang_get_seg_from_uri(req->make_reqpath());
            // played++;
            // dang_tile_buffer=0;
            time_start_play = std::chrono::duration_cast<std::chrono::milliseconds>(
                     get_time() - client->timing.connect_end_time).count();
            struct ev_loop *loop = EV_DEFAULT;
           ev_timer_init(&tev,play_out,0,dang_sd*1.0/1000);
            ev_timer_start (loop, &tev);
            cout<<"pass"<<endl;
          }
          play_file<<played<<endl;
           
           // if(request2==request1){
           //  hung_inst_thrp = dang_thrp1.at(index)+dang_thrp2.at(index);
           //    Dang_algorithm_priority_surveillance(client);
           // }
        



      // Hung: add new requests. Note that if a deadlock happens, check the number of segments in the server
    
      // else {

      //   if(request1==request2){
      //     int index = dang_thrp1.size()-1;
      //     hung_inst_thrp = dang_thrp1.at(index)+dang_thrp2.at(index);
      //   Dang_algorithm_priority(client);
      // }
       // hung_KPush_method(client);
       // num_of_request++;
      }        
      }
      else {
       // need_request++;
       // if(need_request==num_of_tile){

          // int sz = dang_thrp[0].size();
          // hung_inst_thrp=0;
          // for(int i=0;i<num_of_tile;i++)
          //   hung_inst_thrp+=dang_thrp[i].at(sz-1);

        if(req->make_reqpath()=="/terminate_segment"){
          if(curr_segment==seg_terminate){
            canceled=true;
            cout<<"dm cm "<<curr_segment<<endl;
          }

        }
        else{
          dang_inst_thrp = total_data*1000.0/total_time;
          count_tile=0;
          total_rate_downloaded = total_data / (dang_sd/1000);
          dang_segment_rate.push_back(total_rate_downloaded);
         

          Dang_adaptation_vr(client,1);
          total_data=0;
          total_time=0;
      }
          //dang_rst=false;
        //}}
      }

  // Hung: receive stream_id
  std::cout << "[RECV] receive stream " << req->stream_id << " Thrp: " << hung_inst_thrp << " Buff: " << hung_cur_buff << std::endl;
  //print_timer();

  if (client->all_requests_processed()) {
    nghttp2_session_terminate_session(session, NGHTTP2_NO_ERROR);
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
  //ofstream f2;
  std::vector<Request *> reqs;
  reqs.reserve(client.reqvec.size());
  for (const auto &req : client.reqvec) {
    if (req->timing.state == RequestState::ON_COMPLETE) {
      reqs.push_back(req.get());
    }
  }
  // for(int i=0;i<num_of_tile;i++){
  //   // f1.open("/home/haidang/client/Output/priority/video1.txt");
  //   // f2.open("/home/haidang/client/Output/priority/video2.txt");
  //   // f1<<"finishTime\tThrp1\tBitrate1\tBuffer1\tDownloadtime1"<<endl;
  //   // f2<<"finishTime\tThrp2\tBitrate2\tBuffer2\tDownloadtime2"<<endl;
  //   f[i].open("/home/haidang/client/Output/priority/tile"+to_string(i+1)+".txt");
  //   f[i] << "finishTime\tThrp\tBitrate"<<endl;
  // }
  buffering_file.close();
  tile_file.close();
 dang_file.open("/home/haidang/client/Output/results.txt");
  dang_file<<"finishTime\tSeg\tTile\tThrp\tTile rate\tSegment rate\tBuffer"<<endl;
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

id  responseEnd responseStart requestStart  process code size request path)" << std::endl;

  // Hung: 
int index[num_of_tile];
int curr_seg=0;
double thrp_print;
int cnt=0,cnt_buff=0;
for(int i=0;i<num_of_tile;i++)
  index[i]=0;
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
              << ("+" + util::format_duration(response_end)) << " " << std::setw(13)
              << ("+" + util::format_duration(response_start)) << " "
              << (pushed ? "*" : " ") << std::setw(11)
              << ("+" + util::format_duration(request_start)) << " "
              << std::setw(8) << util::format_duration(total) << " "
              << std::setw(4) << req->status << " " << std::setw(4)
              << util::utos_unit(req->response_len) << " "
              << req->make_reqpath() << std::endl;
    // if(req->stream_id%2==0){
    //   int tile = dang_get_video_from_uri(req->make_reqpath());
    //   dang_bitrate[tile-1].push_back(dang_get_rate_from_uri(req->make_reqpath()));
    //   f[tile-1]<<response_end.count()*1.0/1000000<<'\t'<<
    //            dang_thrp[tile-1].at(index[tile-1])<<'\t'<<
    //           dang_get_rate_from_uri(req->make_reqpath())<<endl;
    //           index[tile-1]++;
    //   buffer<<response_end.count()*1.0/1000000<<'\t'<<
    //           dang_buffer.at(curr_seg)*1.0/1000<<endl;
    //           curr_seg++;
             
                
    // }
              if(req->stream_id%2==0){

               // if(dang_get_video_from_uri(req->make_reqpath())==num_of_tile){
                  thrp_print=dang_thrp_record.at(cnt);
                  cnt++;
               // }
                dang_file<<response_end.count()*1.0/1000000<<'\t'
                <<dang_get_seg_from_uri(req->make_reqpath())<<'\t'
                <<dang_get_video_from_uri(req->make_reqpath())<<'\t'
                
                <<thrp_print<<'\t'
                <<req->response_len * 8.0/1000<<'\t'
                <<dang_segment_rate.at(dang_get_seg_from_uri(req->make_reqpath())-1)<<'\t'
                <<dang_tile_buffer_record.at(cnt_buff)<<endl;
                cnt_buff++;
              }
               
}
    // for (const auto &req : reqs)
    // if(req->stream_id%2==0){
    //    auto response_end = std::chrono::duration_cast<std::chrono::microseconds>(
    //     req->timing.response_end_time - base);
    // auto response_start = std::chrono::duration_cast<std::chrono::microseconds>(
    //     req->timing.response_start_time - base);
    // auto request_start = std::chrono::duration_cast<std::chrono::microseconds>(
    //     req->timing.request_start_time - base);
    // auto total = std::chrono::duration_cast<std::chrono::microseconds>(
    //     req->timing.response_end_time - req->timing.request_start_time);
    // auto pushed = req->stream_id % 2 == 0;
    //             // if(request_corr_id[req->stream_id]==1){
    //             //   f1<<response_end.count()*1.0/1000000 << "\t"<< dang_thrp1.at(j1)*1.0 <<"\t"<<req ->response_len*8/1000<<endl;
    //             //   j1++;
    //             // }
    //             // else{
    //             //   f2<<response_end.count()*1.0/1000000 << "\t"<< dang_thrp2.at(j2)*1.0 <<"\t"<<req ->response_len*8/1000<<endl;
    //             //   j2++;
    //             // }
    //             auto download_time = (response_end-response_start).count(); 
    //             // if(dang_get_video_from_uri(req->make_reqpath())==1){
    //             //   f1<<response_end.count()*1.0/1000000 << "\t"<< dang_thrp1.at(j1)*1.0 <<"\t"<<req ->response_len*8/hung_sd<<'\t'<<buffer1.at(j1)<<'\t'<<download_time<<endl;
    //             //   cout<<response_end.count()*1.0/1000000 << "\t"<< dang_thrp1.at(j1)*1.0 <<"\t"<<req ->response_len*8/hung_sd<<'\t'<<buffer1.at(j1)<<endl;
    //             //   j1++;
    //             // }
    //             // else{
    //             //   f2<<response_end.count()*1.0/1000000 << "\t"<< dang_thrp2.at(j2)*1.0 <<"\t"<<req ->response_len*8/hung_sd<<'\t'<<buffer2.at(j2)<<'\t'<<download_time<<endl;
    //             //   cout<<response_end.count()*1.0/1000000 << "\t"<< dang_thrp2.at(j2)*1.0 <<"\t"<<req ->response_len*8/hung_sd<<'\t'<<buffer2.at(j2)<<endl;
    //             //   j2++;
    //             // }
    //       }
  // Hung: Doan nay tinh thoi gian, sau do in ra (sua roi nhe)
//  ofstream MyExcelFile,log;
//  string name = "/home/haidang/Output/rtt/seamless"+to_string(hung_cur_rtt);
//     MyExcelFile.open(name+".xlsx");
//     MyExcelFile << "Time, Thrp, Bitrate, Buffer, Download time" << endl;
//   std::cout << std::endl << "Our statistics: " << std::endl;
//   std::cout << "Index \tTime \tThrp \tRate \tBuffer" << std::endl;
//   for (int i = 0; i < hung_time_recorder.size(); i++) {
//     std::cout << hung_seg_recorder.at(i) << "\t"
//               << hung_time_recorder.at(i)/1000.0 << "\t"
//               << hung_thrp_recorder.at(i) << "\t"
//               << hung_rate_recorder.at(i) << "\t"
//               << hung_buff_recorder.at(i)/1000.0 << "\t" 
//               <<std::endl;

    

    
//     MyExcelFile << hung_time_recorder.at(i)/1000.0<<','<<hung_thrp_recorder.at(i)<<','<<hung_rate_recorder.at(i) << ','<<hung_buff_recorder.at(i)/1000.0<<','<<time_download_recorder.at(i)/1000<<endl;//<<','<<thrp_est_recorder.at(i)<<','<<smooth_thrp_recoder.at(i)<<endl;
    
//   }
//   double buff_avg=0, rate_avg=0;
//   int num_of_bufferMin = 0;
//   int lowest_buff = 100000;
//   int lowest_rate = 100000;
//   int highest_buff = -1;
//   int num_of_switch_down =0;
//   int switch_down_greater_3=0;
//   int max_step=-1;
//   int num_of_segment = hung_buff_recorder.size();
//   int buff_min=5000;
//   int rate_down_avg=0;
//   for (int i = 0; i < num_of_segment; i++){
//         buff_avg+=hung_buff_recorder.at(i);
//         rate_avg += hung_rate_recorder.at(i);
//         if (hung_buff_recorder.at(i)< buff_min&&i > hung_tar_buff/hung_sd + 1) num_of_bufferMin ++;
//         if (hung_buff_recorder.at(i) < lowest_buff &&i > hung_tar_buff/hung_sd + 1) lowest_buff =hung_buff_recorder.at(i);
//         if(hung_rate_recorder.at(i) <lowest_rate && i > hung_tar_buff/hung_sd + 1) lowest_rate=hung_rate_recorder.at(i);
//          if (hung_buff_recorder.at(i) > highest_buff) highest_buff =hung_buff_recorder.at(i);
//          if(i<num_of_segment-1){
//           if(hung_rate_recorder.at(i)>hung_rate_recorder.at(i+1)){
//             num_of_switch_down++;
//             int old_version =getIndexByRate(hung_rate_recorder.at(i));
//             int new_version=getIndexByRate(hung_rate_recorder.at(i+1));
//             if(old_version-new_version > max_step) max_step=old_version-new_version ;
//             if(old_version-new_version>=3) switch_down_greater_3 ++;
//             rate_down_avg+=hung_rate_recorder.at(i)-hung_rate_recorder.at(i+1);
//           }
//       }
//   }
  
//   MyExcelFile.close();
//   log.open(name+".txt");
//   log << "rate avg : " << std::setw(15) << rate_avg/num_of_segment<<endl;
//   log << "buff avg : " << std::setw(15) << buff_avg/num_of_segment<<endl;
//   log << "lowest rate : " <<std::setw(15) << lowest_rate<<endl;
//   log << "lowest buffer : " <<std::setw(15) << lowest_buff<<endl;
//   log << "highest buffer : " <<std::setw(15) << highest_buff<<endl;
//   log << "num of buffer under min : " << std::setw(15) << num_of_bufferMin<<endl;
//   log << "num of switch down : " <<std::setw(15) <<  num_of_switch_down<<endl;
//   log << "rate switch down avg : "<<std::setw(15) << rate_down_avg*1.0/num_of_switch_down<<endl;
//   log << "max step down : " << std::setw(15)<<max_step<<" version"<<endl;
//   log << "num switch down >= 3 : "<<std::setw(15)<<switch_down_greater_3 <<endl;
//   log << "num of request : "<< std::setw(15)<<num_of_request;

//   log.close();
  //  for(int i=0;i<num_of_tile;i++)
  //   f[i].close();
  // buffer.close();
  dang_file.close();
  cout<<"finish"<<endl;
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
        //client.add_request(std::get<0>(req), std::get<1>(req), std::get<2>(req),
        //                   pri_spec);

        // Hung_comment: These parameters are too complicated thus saving them is the best solution
        dang_data_prd = std::get<1>(req);
        dang_data_length = std::get<2>(req);
        dang_pri_spec = pri_spec; 
        client.add_request(std::get<0>(req), std::get<1>(req), std::get<2>(req),
                       pri_spec);
  
        // hung_req_vod_rebuff(&client, false);
        // break;

      }
    }
    request_corr_id[13]=1;
    request_corr_id[15]=2;
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
 //cout<<"finish"<<endl;
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
  cout<<"ok"<<endl;
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
              HTTP/2 dependency  based priority.  The  priority order,
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
    int c = getopt_long(argc, argv, "M:Oab:c:d:gm:np:r:hH:vst:uw:W:",
                        long_options, &option_index);
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
  //cout<<"ok"<<endl;
  return run(argv + optind, argc - optind);
}

} // namespace nghttp2
// void play_out12(struct ev_loop *loop, ev_timer *w, int revents) {
//  cout<<"timer"<<endl;
// }
int main(int argc, char **argv) {
  // Hung:
  //if (system("./start.sh &")) {std::cout << "could not run DummyNet" << std::endl; } 
 // cout<<a<<endl;
  //init();
if (system("sudo ./complex_.sh &")) {std::cout << "could not run DummyNet" << std::endl; }

  return nghttp2::run_app(nghttp2::main, argc, argv);
  // cout<<"abc"<<endl;
  // ev_timer vtev;
  // struct ev_loop *loop = EV_DEFAULT;
 //     ev_timer_init(&vtev,play_out12,0,1);
 //     ev_timer_start (loop, &vtev);

}