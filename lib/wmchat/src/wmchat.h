// wmchat.h
//
// Copyright (c) 2020-2023 Kristofer Berggren
// All rights reserved.
//
// nchat is distributed under the MIT license, see LICENSE for details.

#pragma once

#include <condition_variable>
#include <deque>
#include <map>
#include <thread>

#include "protocol.h"

class WmChat : public Protocol
{
public:
  WmChat();
  virtual ~WmChat();
  static std::string GetName() { return "WhatsAppMd"; }
  static std::string GetLibName() { return "libwmchat"; }
  static std::string GetCreateFunc() { return "CreateWmChat"; }
  std::string GetProfileId() const;
  bool HasFeature(ProtocolFeature p_ProtocolFeature) const;

  bool SetupProfile(const std::string& p_ProfilesDir, std::string& p_ProfileId);
  bool LoadProfile(const std::string& p_ProfilesDir, const std::string& p_ProfileId);
  bool CloseProfile();

  bool Login();
  bool Logout();

  void Process();

  void SendRequest(std::shared_ptr<RequestMessage> p_RequestMessage);
  void SetMessageHandler(const std::function<void(std::shared_ptr<ServiceMessage>)>& p_MessageHandler);

public:
  static void AddInstance(int p_ConnId, WmChat* p_Instance);
  static void RemoveInstance(int p_ConnId);
  static WmChat* GetInstance(int p_ConnId);

private:
  void CallMessageHandler(std::shared_ptr<ServiceMessage> p_ServiceMessage);
  void PerformRequest(std::shared_ptr<RequestMessage> p_RequestMessage);
  std::string GetProxyUrl() const;

private:
  std::string m_ProfileId;
  std::function<void(std::shared_ptr<ServiceMessage>)> m_MessageHandler;

  bool m_Running = false;
  std::thread m_Thread;
  std::deque<std::shared_ptr<RequestMessage>> m_RequestsQueue;
  std::mutex m_ProcessMutex;
  std::condition_variable m_ProcessCondVar;

  static std::mutex s_ConnIdMapMutex;
  static std::map<int, WmChat*> s_ConnIdMap;
  int m_ConnId = -1;
  std::string m_ProfileDir;
  static const int s_CacheDirVersion = 0;
};

extern "C" {
void WmNewContactsNotify(int p_ConnId, char* p_ChatId, char* p_Name, int p_IsSelf);
void WmNewChatsNotify(int p_ConnId, char* p_ChatId, int p_IsUnread, int p_IsMuted, int p_LastMessageTime);
void WmNewMessagesNotify(int p_ConnId, char* p_ChatId, char* p_MsgId, char* p_SenderId, char* p_Text, int p_FromMe,
                         char* p_ReplyId, char* p_FileId, char* p_FilePath, int p_FileStatus, int p_TimeSent,
                         int p_IsRead);
void WmNewStatusNotify(int p_ConnId, char* p_ChatId, char* p_UserId, int p_IsOnline, int p_IsTyping, int p_TimeSeen);
void WmNewMessageStatusNotify(int p_ConnId, char* p_ChatId, char* p_MsgId, int p_IsRead);
void WmNewMessageFileNotify(int p_ConnId, char* p_ChatId, char* p_MsgId, char* p_FilePath, int p_FileStatus,
                            int p_Action);
void WmSetStatus(int p_Flags);
void WmClearStatus(int p_Flags);
void WmLogTrace(char* p_Filename, int p_LineNo, char* p_Message);
void WmLogDebug(char* p_Filename, int p_LineNo, char* p_Message);
void WmLogInfo(char* p_Filename, int p_LineNo, char* p_Message);
void WmLogWarning(char* p_Filename, int p_LineNo, char* p_Message);
void WmLogError(char* p_Filename, int p_LineNo, char* p_Message);
}

extern "C" WmChat* CreateWmChat();
