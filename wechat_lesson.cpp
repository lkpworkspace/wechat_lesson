/****************************************************************************
Copyright (c) 2018, likepeng
All rights reserved.

Author: likepeng <likepeng0418@163.com>
****************************************************************************/
#include <memory>
#include <chrono>
#include <thread>

#include <glog/logging.h>

#include "myframe/msg.h"
#include "myframe/actor.h"
#include "myframe/worker.h"

#include "http.pb.h"

class wechat_lessonActor : public myframe::Actor {
 public:
  int Init(const char* param) override {
    return 0;
  }

  void Proc(const std::shared_ptr<const myframe::Msg>& msg) override {
    std::shared_ptr<myframe::HttpReq> http_req = nullptr;
    try {
      http_req = msg->GetAnyData<std::shared_ptr<myframe::HttpReq>>();
    } catch (std::exception& e) {
      LOG(ERROR) << "get http req failed";
      return;
    }
    // 打印http请求
    LOG(INFO) << *msg << " " << http_req->DebugString();
  }
};

/* 创建actor实例函数 */
extern "C" std::shared_ptr<myframe::Actor> my_actor_create(
    const std::string& actor_name) {
  if (actor_name == "wechat_lesson") {
    return std::make_shared<wechat_lessonActor>();
  }
  return nullptr;
}
