#include <iostream>
#include <chrono>
#include <thread>
#include <mutex>
#include <boost/algorithm/string.hpp>
#include <boost/bind.hpp>
#include <nghttp2/asio_http2_server.h>
//using namespace std;
using namespace nghttp2::asio_http2;
using namespace nghttp2::asio_http2::server;

std::string bitrate[2];

int segment_duration = 500; // 1000ms
// auto avail_seg = std::make_shared<int>();
// auto server_seg = std::make_shared<int>();
// auto client_seg = std::make_shared<int>();
int avail_seg[2],server_seg[2],client_seg[2];
const int MAX_SEGMENTS = 251;

bool on_periodic_mode[2] = {true,true};
bool on_pushing_in_periodic_mode[2] = {false,false};
bool on_steady_stage[2] = {false,false};

std::chrono::time_point<std::chrono::system_clock> start[2], end;

void init_new_connection(int video) {
  avail_seg[video-1] = 0;

  on_periodic_mode [video-1]= true;
  on_pushing_in_periodic_mode[video-1] = false;

   start[video-1] = std::chrono::system_clock::now();
}


int get_time(int video) {
  end = std::chrono::system_clock::now();
  return std::chrono::duration_cast<std::chrono::milliseconds>(end - start[video-1]).count();
}

void print_new_seg(int cur_seg,int video) {
  if (on_periodic_mode[video-1])
    std::cout << std::endl << "Push seg #" << cur_seg <<" of video "<<video<< " bitrate " << bitrate[video-1];
  else 
    std::cout << std::endl << "Respond seg #" << cur_seg <<" of video "<<video<<" bitrate " << bitrate[video-1];

  std::cout << " at time: " << get_time(video) << "ms" << std::endl;
}

void push_remaining_files1(const response *res) {
  if (server_seg[0] + 1 >= MAX_SEGMENTS) {
    res->write_head(200);
    res->end(file_generator("push.html"));
    return;
  } 

  print_new_seg(server_seg[0]+1,1);
  boost::system::error_code ec;

  auto push = res->push(ec, "GET", "/seg_"+std::to_string(server_seg[0]+1)+"_rate_"+bitrate[0]+"_video_1");

  push->on_close([res](uint32_t error_code) {
    if (!error_code) {                    // if CANCEL is not sent
      server_seg[0]++;
      std::cout << "Sent seg #" << server_seg[0] << " bitrate " << bitrate[0]
                        << " of video 1 at time: " << get_time(1) << "ms" << std::endl;
    }

    if (error_code == NGHTTP2_CANCEL) {   // if receving Cancel req (code = 0x08)
      server_seg[0] = client_seg[0];
      std::cout << "Resend seg #" << server_seg[0]+1 << " bitrate " << bitrate[0]
                        << " of video 1 at time: " << get_time(1) << "ms" << std::endl;
    } 

    if (server_seg[0] >= avail_seg[0]) {
      on_periodic_mode[0] = true;
      on_pushing_in_periodic_mode[0] = false;
    } else {
      on_periodic_mode[0] = false;
      on_pushing_in_periodic_mode[0] = true;
      push_remaining_files1(res);
    }

  });

  push->write_head(200);
  push->end(file_generator("./real_cbr/"+std::to_string(segment_duration)+"ms/"+bitrate[0]));
  on_pushing_in_periodic_mode[0] = true;
}

void push_file1(
    std::shared_ptr<boost::asio::basic_deadline_timer<boost::posix_time::ptime>> timer, 
    const response *res, std::shared_ptr<bool> closed, boost::system::error_code &ec) {
  if (ec || *closed) {
    return;
  }

  // Hung_comment: Cai nay chi la dia chi thoi nhe!
  timer->expires_at(timer->expires_at() + boost::posix_time::milliseconds(segment_duration));
  timer->async_wait(boost::bind(push_file1, timer, res, closed, ec));

  avail_seg[0] = avail_seg[0] + 1;          // generate a new segment every segment duration

  // if the server are pushing another segment
  if (on_pushing_in_periodic_mode[0]) { return; }

  // push all remaining segments, 
  // during this duration, on_pushing_in_periodic_mode = true
  push_remaining_files1(res);
}

void push_remaining_files2(const response *res) {
  if (server_seg[1] + 1 >= MAX_SEGMENTS) {
    res->write_head(200);
    res->end(file_generator("push.html"));
    return;
  } 

  print_new_seg(server_seg[1]+1,2);
  boost::system::error_code ec;

  auto push = res->push(ec, "GET", "/seg_"+std::to_string(server_seg[1]+1)+"_rate_"+bitrate[1]+"_video_2");

  push->on_close([res](uint32_t error_code) {
    if (!error_code) {                    // if CANCEL is not sent
      server_seg[1]++;
      std::cout << "Sent seg #" << server_seg[1] << " bitrate " << bitrate [1]
                        << " of video 2 at time: " << get_time(2) << "ms" << std::endl;
    }

    if (error_code == NGHTTP2_CANCEL) {   // if receving Cancel req (code = 0x08)
      server_seg[1] = client_seg[1];
      std::cout << "Resend seg #" << server_seg[1]+1 << " bitrate " << bitrate[1]
                        << " of video 2 at time: " << get_time(2) << "ms" << std::endl;
    } 

    if (server_seg[1] >= avail_seg[1]) {
      on_periodic_mode[1] = true;
      on_pushing_in_periodic_mode[1] = false;
    } else {
      on_periodic_mode[1] = false;
      on_pushing_in_periodic_mode[1] = true;
      push_remaining_files2(res);
    }

  });

  push->write_head(200);
  push->end(file_generator("./real_cbr/"+std::to_string(segment_duration)+"ms/"+bitrate[1]));
  on_pushing_in_periodic_mode[1] = true;
}

void push_file2(
    std::shared_ptr<boost::asio::basic_deadline_timer<boost::posix_time::ptime>> timer, 
    const response *res, std::shared_ptr<bool> closed, boost::system::error_code &ec) {
  if (ec || *closed) {
    return;
  }

  // Hung_comment: Cai nay chi la dia chi thoi nhe!
  timer->expires_at(timer->expires_at() + boost::posix_time::milliseconds(segment_duration));
  timer->async_wait(boost::bind(push_file2, timer, res, closed, ec));

  avail_seg[1] = avail_seg[1] + 1;          // generate a new segment every segment duration

  // if the server are pushing another segment
  if (on_pushing_in_periodic_mode[1]) { return; }

  // push all remaining segments, 
  // during this duration, on_pushing_in_periodic_mode = true
  push_remaining_files2(res);
}


int main(int argc, char *argv[]) {
  boost::system::error_code ec;
  boost::asio::ssl::context tls(boost::asio::ssl::context::sslv23);

//  tls.use_private_key_file("server.key", boost::asio::ssl::context::pem);
//  tls.use_certificate_chain_file("server.crt");
//  configure_tls_context_easy(ec, tls);

  http2 server;
  server.num_threads(100);

  server.handle("/rebuff1/", [](const request &req, const response &res) {
    // get url, e.g. http://127.0.0.1:3002/rebuff/2000 -> bitrate = 2000
    // currently, the special symbol & is not allowed in the url
    std::vector<std::string> strs;
    boost::split(strs, req.uri().path, boost::is_any_of("/"));
    bitrate[0] = strs[2];

    std::cout << "[JUMP] from segment (C)#" << client_seg[0] 
              << " and (S)#" << server_seg[0] 
              << " to segment #" << avail_seg[0] << std::endl;

    server_seg[0] = avail_seg[0];
    client_seg[0] = avail_seg[0];

    if (!on_steady_stage[0]) {
      init_new_connection(1);

      boost::system::error_code ec;

      auto timer = std::make_shared<boost::asio::deadline_timer>(res.io_service(), 
                        boost::posix_time::milliseconds(segment_duration));
      auto closed = std::make_shared<bool>();

      res.on_close([timer, closed](uint32_t error_code) {
        timer->cancel();
        *closed = true;
      });

      timer->async_wait(boost::bind(push_file1, timer, &res, closed, ec));

      on_steady_stage[0] = true;

    } else {
      res.write_head(200);
      res.end("Responded by "+bitrate[0]);
    }

  });

  server.handle("/rebuff2/", [](const request &req, const response &res) {
    // get url, e.g. http://127.0.0.1:3002/rebuff/2000 -> bitrate = 2000
    // currently, the special symbol & is not allowed in the url
    std::vector<std::string> strs;
    boost::split(strs, req.uri().path, boost::is_any_of("/"));
    bitrate[1] = strs[2];

    std::cout << "[JUMP] from segment (C)#" << client_seg[1] 
              << " and (S)#" << server_seg[1] 
              << " to segment #" << avail_seg[1] << std::endl;

    server_seg[1] = avail_seg[1];
    client_seg[1] = avail_seg[1];

    if (!on_steady_stage[1]) {
      init_new_connection(2);

      boost::system::error_code ec;

      auto timer = std::make_shared<boost::asio::deadline_timer>(res.io_service(), 
                        boost::posix_time::milliseconds(segment_duration));
      auto closed = std::make_shared<bool>();

      res.on_close([timer, closed](uint32_t error_code) {
        timer->cancel();
        *closed = true;
      });

      timer->async_wait(boost::bind(push_file2, timer, &res, closed, ec));

      on_steady_stage[1] = true;

    } else {
      res.write_head(200);
      res.end("Responded by "+bitrate[1]);
    }

  });

  server.handle("/change1/", [](const request &req, const response &res) {
    std::vector<std::string> strs;
    boost::split(strs, req.uri().path, boost::is_any_of("/"));
    bitrate[0] = strs[2];

    std::cout << "[CONT] segment #" << server_seg[0]+1 << std::endl;

    res.write_head(200);
    res.end("Responded by "+bitrate[0]);
  });

    server.handle("/change2/", [](const request &req, const response &res) {
    std::vector<std::string> strs;
    boost::split(strs, req.uri().path, boost::is_any_of("/"));
    bitrate[1] = strs[2];

    std::cout << "[CONT] segment #" << server_seg[1]+1 << std::endl;

    res.write_head(200);
    res.end("Responded by "+bitrate[1]);
  });

  server.handle("/req1/", [](const request &req, const response &res) {
    std::vector<std::string> strs;
    boost::split(strs, req.uri().path, boost::is_any_of("/"));
    bitrate[0] = strs[2];

    client_seg[0] = std::stoi(strs[3]);
    // Now, we did not consider the case that the server sent out the cancel segment,
    // when RTT is changed
    std::cout << "[SUCC] segment #" << client_seg[0] << std::endl;

    res.write_head(200);
    res.end("Responded by "+bitrate[0]);
  });

   server.handle("/req2/", [](const request &req, const response &res) {
    std::vector<std::string> strs;
    boost::split(strs, req.uri().path, boost::is_any_of("/"));
    bitrate[1] = strs[2];

    client_seg[1] = std::stoi(strs[3]);
    // Now, we did not consider the case that the server sent out the cancel segment,
    // when RTT is changed
    std::cout << "[SUCC] segment #" << client_seg[1] << std::endl;

    res.write_head(200);
    res.end("Responded by "+bitrate[1]);
  });

/*
  std::string style_css = "h1 { color: red; }";
  server.handle("/push.html", [&style_css](const request &req, const response &res) {
    boost::system::error_code ec;

    res.write_head(200);
    res.end(file_generator("push.html"));

    auto push = res.push(ec, "GET", "/style.css");
    push->write_head(200);
    push->end(style_css);

    auto push2 = res.push(ec, "GET", "/push.png");
    push2->write_head(200);
    push2->end(file_generator("push.png"));
  });
*/

  std::cout << "Listening at the address: " << "172.16.165.130" << std::endl;
  if (server.listen_and_serve(ec, "172.16.165.130", "3002")) {
    std::cerr << "error: " << ec.message() << std::endl;
  }

/*
  if (server.listen_and_serve(ec, tls, "192.168.9.76", "3002")) {
    std::cerr << "error: " << ec.message() << std::endl;
  }
*/
}
