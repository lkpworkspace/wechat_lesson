/****************************************************************************
Copyright (c) 2023, likepeng
All rights reserved.

Author: likepeng <likepeng0418@163.com>
****************************************************************************/
#pragma once
#include <chrono>
#include <pugixml.hpp>
#include <glog/logging.h>

/**
 * 解析接收到的消息
 * 构造发送消息
 */
class WxMsg {
  pugi::xml_document xml_doc_;
  pugi::xml_node xml_node_;

 public:
  explicit WxMsg() = default;

  void SetXmlNode(const pugi::xml_node& xml_node) {
    xml_node_ = xml_node;
  }

  std::string Get(const std::string& name) const {
    return xml_node_.child(name.c_str()).child_value();
  }
};

class WxMsgParser {
 public:
  static bool Parse(const std::string& xml_str, WxMsg* msg) {
    pugi::xml_document xml_doc;
    auto des = xml_doc.load_buffer(xml_str.c_str(), xml_str.size());
    if (!des) {
      LOG(ERROR) << "xml doc error";
      return false;
    }
    if (xml_doc.child("xml").empty()) {
      LOG(ERROR) << "no xml node";
      return false;
    }
    auto xml_node = xml_doc.child("xml");
    if (xml_node.child("CreateTime").empty()) {
      LOG(ERROR) << "no create time";
      return false;
    }
    std::string self_id = xml_node.child("ToUserName").child_value();
    std::string user_id = xml_node.child("FromUserName").child_value();
    std::string msg_type = xml_node.child("MsgType").child_value();
    if (self_id.empty() || user_id.empty() || msg_type.empty()) {
      LOG(ERROR) << "some key is empty";
      LOG(ERROR) << self_id << "," << user_id << "," << msg_type;
      return false;
    }
    msg->SetXmlNode(xml_node);
    return true;
  }
};

class WxMsgBuilder {
 public:
  static std::string BuildText(
    const std::string& self_id,
    const std::string& user_id,
    const std::string& content) {
    pugi::xml_document xml_doc;
    auto xml_node = xml_doc.append_child("xml");
    xml_node.append_child("ToUserName").append_child(pugi::node_cdata).set_value(user_id.c_str());
    xml_node.append_child("FromUserName").append_child(pugi::node_cdata).set_value(self_id.c_str());
    auto now = std::chrono::system_clock::now();
    auto str_time = std::to_string(int(now.time_since_epoch().count() / 1e9));
    auto time_node = xml_node.append_child("CreateTime").append_buffer(str_time.c_str(), str_time.size());
    xml_node.append_child("MsgType").append_child(pugi::node_cdata).set_value("text");
    xml_node.append_child("Content").append_child(pugi::node_cdata).set_value(content.c_str());
    std::stringstream ss;
    xml_doc.save(ss);
    return ss.str();
  }
};
