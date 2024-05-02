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

#include "myframe_pb/http.pb.h"
#include "wx_msg.h"

class wechat_lessonActor : public myframe::Actor {
 public:
  int Init(const char* param) override {
    Subscribe("actor.wspp_server.wspp_server");
    return 0;
  }

  void Proc(const std::shared_ptr<const myframe::Msg>& msg) override {
    std::shared_ptr<myframe::HttpReq> http_req = nullptr;
    try {
      http_req = msg->GetAnyData<std::shared_ptr<myframe::HttpReq>>();
    } catch (std::exception& e) {
      LOG(ERROR) << "get http req failed";
      ReplyHttp(msg->GetSrc(), 404, "");
      return;
    }
    // 打印http请求
    LOG(INFO) << *msg << " " << http_req->DebugString();
    // 解析uri中的参数
    Json::Value resource;
    if (!ParseResourceJson(http_req->uri(), &resource)) {
      LOG(ERROR) << "parse resource failed";
      ReplyHttp(msg->GetSrc(), 404, "");
      return;
    }
    // 判断是否微信服务器验证消息
    if (resource.isMember("params") && resource["params"].isMember("echostr")) {
      ReplyHttp(msg->GetSrc(), 200, resource["params"]["echostr"].asString());
      return;
    }
    // 解析微信服务器消息并回显
    WxMsg wxmsg;
    if (!WxMsgParser::Parse(http_req->body(), &wxmsg)) {
      ReplyHttp(msg->GetSrc(), 404, "");
      return;
    }
    auto resp_body = WxMsgBuilder::BuildText(
      wxmsg.Get("ToUserName"),
      wxmsg.Get("FromUserName"),
      wxmsg.Get("Content"));
    ReplyHttp(msg->GetSrc(), 200, resp_body);
  }

  void ReplyHttp(const std::string& dst, int code, const std::string& body) {
    auto resp = std::make_shared<myframe::HttpResp>();
    resp->set_code(code);
    resp->set_body(body);
    LOG(INFO)
      << "resp http msg code "
      << resp->code()
      << (resp->body().size() > 512 ? "" : (", body: " + resp->body()));
    auto msg = std::make_shared<myframe::Msg>();
    msg->SetAnyData(resp);
    auto mailbox = GetMailbox();
    mailbox->Send(dst, msg);
  }

  std::vector<std::string> Split(const std::string& str, char delim) {
    std::vector<std::string> name_list;
    std::string item;
    std::stringstream ss(str);
    while (std::getline(ss, item, delim)) {
      name_list.push_back(item);
    }
    return name_list;
  }

  /*
  将resource解析成如下格式的json数据结构
  {
    "path":"",
    "params":{
      "":"",
      "",""
    }
  }
  */
  bool ParseResourceJson(const std::string& s, Json::Value* jv_ptr) {
    auto& v = *jv_ptr;
    auto pos = s.find('?');
    if (pos == s.npos) {
      // TODO check path is ok
      v["path"] = s;
      return true;
    }
    // TODO check path is ok
    v["path"] = s.substr(0, pos);
    auto params_str = s.substr(pos + 1);
    // spilt params
    auto param_pair = Split(params_str, '&');
    for (int i = 0; i < param_pair.size(); ++i) {
      auto pair = Split(param_pair[i], '=');
      if (pair.size() != 2) {
        LOG(ERROR) << "param fmt error";
        return false;
      }
      v["params"][pair[0]] = pair[1];
    }
    return true;
  }
};

/* 创建actor实例函数 */
extern "C" std::shared_ptr<myframe::Actor> actor_create(
    const std::string& actor_name) {
  if (actor_name == "wechat_lesson") {
    return std::make_shared<wechat_lessonActor>();
  }
  return nullptr;
}
