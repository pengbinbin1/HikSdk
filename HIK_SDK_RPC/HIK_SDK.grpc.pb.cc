// Generated by the gRPC C++ plugin.
// If you make any local change, they will be lost.
// source: HIK_SDK.proto

#include "HIK_SDK.pb.h"
#include "HIK_SDK.grpc.pb.h"

#include <functional>
#include <grpcpp/impl/codegen/async_stream.h>
#include <grpcpp/impl/codegen/async_unary_call.h>
#include <grpcpp/impl/codegen/channel_interface.h>
#include <grpcpp/impl/codegen/client_unary_call.h>
#include <grpcpp/impl/codegen/client_callback.h>
#include <grpcpp/impl/codegen/method_handler_impl.h>
#include <grpcpp/impl/codegen/rpc_service_method.h>
#include <grpcpp/impl/codegen/service_type.h>
#include <grpcpp/impl/codegen/sync_stream.h>
namespace HIK_SDK_P {

static const char* HIKSDK_method_names[] = {
  "/HIK_SDK_P.HIKSDK/Init",
  "/HIK_SDK_P.HIKSDK/Uninit",
  "/HIK_SDK_P.HIKSDK/SaveFileByTime",
  "/HIK_SDK_P.HIKSDK/SaveFileByName",
  "/HIK_SDK_P.HIKSDK/FindFileByTime",
  "/HIK_SDK_P.HIKSDK/GetDVRConfig",
};

std::unique_ptr< HIKSDK::Stub> HIKSDK::NewStub(const std::shared_ptr< ::grpc::ChannelInterface>& channel, const ::grpc::StubOptions& options) {
  (void)options;
  std::unique_ptr< HIKSDK::Stub> stub(new HIKSDK::Stub(channel));
  return stub;
}

HIKSDK::Stub::Stub(const std::shared_ptr< ::grpc::ChannelInterface>& channel)
  : channel_(channel), rpcmethod_Init_(HIKSDK_method_names[0], ::grpc::internal::RpcMethod::NORMAL_RPC, channel)
  , rpcmethod_Uninit_(HIKSDK_method_names[1], ::grpc::internal::RpcMethod::NORMAL_RPC, channel)
  , rpcmethod_SaveFileByTime_(HIKSDK_method_names[2], ::grpc::internal::RpcMethod::NORMAL_RPC, channel)
  , rpcmethod_SaveFileByName_(HIKSDK_method_names[3], ::grpc::internal::RpcMethod::NORMAL_RPC, channel)
  , rpcmethod_FindFileByTime_(HIKSDK_method_names[4], ::grpc::internal::RpcMethod::NORMAL_RPC, channel)
  , rpcmethod_GetDVRConfig_(HIKSDK_method_names[5], ::grpc::internal::RpcMethod::NORMAL_RPC, channel)
  {}

::grpc::Status HIKSDK::Stub::Init(::grpc::ClientContext* context, const ::HIK_SDK_P::LoginInfo& request, ::HIK_SDK_P::ErrCode* response) {
  return ::grpc::internal::BlockingUnaryCall(channel_.get(), rpcmethod_Init_, context, request, response);
}

void HIKSDK::Stub::experimental_async::Init(::grpc::ClientContext* context, const ::HIK_SDK_P::LoginInfo* request, ::HIK_SDK_P::ErrCode* response, std::function<void(::grpc::Status)> f) {
  return ::grpc::internal::CallbackUnaryCall(stub_->channel_.get(), stub_->rpcmethod_Init_, context, request, response, std::move(f));
}

::grpc::ClientAsyncResponseReader< ::HIK_SDK_P::ErrCode>* HIKSDK::Stub::AsyncInitRaw(::grpc::ClientContext* context, const ::HIK_SDK_P::LoginInfo& request, ::grpc::CompletionQueue* cq) {
  return ::grpc::internal::ClientAsyncResponseReaderFactory< ::HIK_SDK_P::ErrCode>::Create(channel_.get(), cq, rpcmethod_Init_, context, request, true);
}

::grpc::ClientAsyncResponseReader< ::HIK_SDK_P::ErrCode>* HIKSDK::Stub::PrepareAsyncInitRaw(::grpc::ClientContext* context, const ::HIK_SDK_P::LoginInfo& request, ::grpc::CompletionQueue* cq) {
  return ::grpc::internal::ClientAsyncResponseReaderFactory< ::HIK_SDK_P::ErrCode>::Create(channel_.get(), cq, rpcmethod_Init_, context, request, false);
}

::grpc::Status HIKSDK::Stub::Uninit(::grpc::ClientContext* context, const ::HIK_SDK_P::Empty& request, ::HIK_SDK_P::ErrCode* response) {
  return ::grpc::internal::BlockingUnaryCall(channel_.get(), rpcmethod_Uninit_, context, request, response);
}

void HIKSDK::Stub::experimental_async::Uninit(::grpc::ClientContext* context, const ::HIK_SDK_P::Empty* request, ::HIK_SDK_P::ErrCode* response, std::function<void(::grpc::Status)> f) {
  return ::grpc::internal::CallbackUnaryCall(stub_->channel_.get(), stub_->rpcmethod_Uninit_, context, request, response, std::move(f));
}

::grpc::ClientAsyncResponseReader< ::HIK_SDK_P::ErrCode>* HIKSDK::Stub::AsyncUninitRaw(::grpc::ClientContext* context, const ::HIK_SDK_P::Empty& request, ::grpc::CompletionQueue* cq) {
  return ::grpc::internal::ClientAsyncResponseReaderFactory< ::HIK_SDK_P::ErrCode>::Create(channel_.get(), cq, rpcmethod_Uninit_, context, request, true);
}

::grpc::ClientAsyncResponseReader< ::HIK_SDK_P::ErrCode>* HIKSDK::Stub::PrepareAsyncUninitRaw(::grpc::ClientContext* context, const ::HIK_SDK_P::Empty& request, ::grpc::CompletionQueue* cq) {
  return ::grpc::internal::ClientAsyncResponseReaderFactory< ::HIK_SDK_P::ErrCode>::Create(channel_.get(), cq, rpcmethod_Uninit_, context, request, false);
}

::grpc::Status HIKSDK::Stub::SaveFileByTime(::grpc::ClientContext* context, const ::HIK_SDK_P::SaveFileParam& request, ::HIK_SDK_P::ErrCode* response) {
  return ::grpc::internal::BlockingUnaryCall(channel_.get(), rpcmethod_SaveFileByTime_, context, request, response);
}

void HIKSDK::Stub::experimental_async::SaveFileByTime(::grpc::ClientContext* context, const ::HIK_SDK_P::SaveFileParam* request, ::HIK_SDK_P::ErrCode* response, std::function<void(::grpc::Status)> f) {
  return ::grpc::internal::CallbackUnaryCall(stub_->channel_.get(), stub_->rpcmethod_SaveFileByTime_, context, request, response, std::move(f));
}

::grpc::ClientAsyncResponseReader< ::HIK_SDK_P::ErrCode>* HIKSDK::Stub::AsyncSaveFileByTimeRaw(::grpc::ClientContext* context, const ::HIK_SDK_P::SaveFileParam& request, ::grpc::CompletionQueue* cq) {
  return ::grpc::internal::ClientAsyncResponseReaderFactory< ::HIK_SDK_P::ErrCode>::Create(channel_.get(), cq, rpcmethod_SaveFileByTime_, context, request, true);
}

::grpc::ClientAsyncResponseReader< ::HIK_SDK_P::ErrCode>* HIKSDK::Stub::PrepareAsyncSaveFileByTimeRaw(::grpc::ClientContext* context, const ::HIK_SDK_P::SaveFileParam& request, ::grpc::CompletionQueue* cq) {
  return ::grpc::internal::ClientAsyncResponseReaderFactory< ::HIK_SDK_P::ErrCode>::Create(channel_.get(), cq, rpcmethod_SaveFileByTime_, context, request, false);
}

::grpc::Status HIKSDK::Stub::SaveFileByName(::grpc::ClientContext* context, const ::HIK_SDK_P::SaveFileParam1& request, ::HIK_SDK_P::ErrCode* response) {
  return ::grpc::internal::BlockingUnaryCall(channel_.get(), rpcmethod_SaveFileByName_, context, request, response);
}

void HIKSDK::Stub::experimental_async::SaveFileByName(::grpc::ClientContext* context, const ::HIK_SDK_P::SaveFileParam1* request, ::HIK_SDK_P::ErrCode* response, std::function<void(::grpc::Status)> f) {
  return ::grpc::internal::CallbackUnaryCall(stub_->channel_.get(), stub_->rpcmethod_SaveFileByName_, context, request, response, std::move(f));
}

::grpc::ClientAsyncResponseReader< ::HIK_SDK_P::ErrCode>* HIKSDK::Stub::AsyncSaveFileByNameRaw(::grpc::ClientContext* context, const ::HIK_SDK_P::SaveFileParam1& request, ::grpc::CompletionQueue* cq) {
  return ::grpc::internal::ClientAsyncResponseReaderFactory< ::HIK_SDK_P::ErrCode>::Create(channel_.get(), cq, rpcmethod_SaveFileByName_, context, request, true);
}

::grpc::ClientAsyncResponseReader< ::HIK_SDK_P::ErrCode>* HIKSDK::Stub::PrepareAsyncSaveFileByNameRaw(::grpc::ClientContext* context, const ::HIK_SDK_P::SaveFileParam1& request, ::grpc::CompletionQueue* cq) {
  return ::grpc::internal::ClientAsyncResponseReaderFactory< ::HIK_SDK_P::ErrCode>::Create(channel_.get(), cq, rpcmethod_SaveFileByName_, context, request, false);
}

::grpc::Status HIKSDK::Stub::FindFileByTime(::grpc::ClientContext* context, const ::HIK_SDK_P::FindFileParam& request, ::HIK_SDK_P::FileData* response) {
  return ::grpc::internal::BlockingUnaryCall(channel_.get(), rpcmethod_FindFileByTime_, context, request, response);
}

void HIKSDK::Stub::experimental_async::FindFileByTime(::grpc::ClientContext* context, const ::HIK_SDK_P::FindFileParam* request, ::HIK_SDK_P::FileData* response, std::function<void(::grpc::Status)> f) {
  return ::grpc::internal::CallbackUnaryCall(stub_->channel_.get(), stub_->rpcmethod_FindFileByTime_, context, request, response, std::move(f));
}

::grpc::ClientAsyncResponseReader< ::HIK_SDK_P::FileData>* HIKSDK::Stub::AsyncFindFileByTimeRaw(::grpc::ClientContext* context, const ::HIK_SDK_P::FindFileParam& request, ::grpc::CompletionQueue* cq) {
  return ::grpc::internal::ClientAsyncResponseReaderFactory< ::HIK_SDK_P::FileData>::Create(channel_.get(), cq, rpcmethod_FindFileByTime_, context, request, true);
}

::grpc::ClientAsyncResponseReader< ::HIK_SDK_P::FileData>* HIKSDK::Stub::PrepareAsyncFindFileByTimeRaw(::grpc::ClientContext* context, const ::HIK_SDK_P::FindFileParam& request, ::grpc::CompletionQueue* cq) {
  return ::grpc::internal::ClientAsyncResponseReaderFactory< ::HIK_SDK_P::FileData>::Create(channel_.get(), cq, rpcmethod_FindFileByTime_, context, request, false);
}

::grpc::Status HIKSDK::Stub::GetDVRConfig(::grpc::ClientContext* context, const ::HIK_SDK_P::ConfParam& request, ::HIK_SDK_P::DeviceList* response) {
  return ::grpc::internal::BlockingUnaryCall(channel_.get(), rpcmethod_GetDVRConfig_, context, request, response);
}

void HIKSDK::Stub::experimental_async::GetDVRConfig(::grpc::ClientContext* context, const ::HIK_SDK_P::ConfParam* request, ::HIK_SDK_P::DeviceList* response, std::function<void(::grpc::Status)> f) {
  return ::grpc::internal::CallbackUnaryCall(stub_->channel_.get(), stub_->rpcmethod_GetDVRConfig_, context, request, response, std::move(f));
}

::grpc::ClientAsyncResponseReader< ::HIK_SDK_P::DeviceList>* HIKSDK::Stub::AsyncGetDVRConfigRaw(::grpc::ClientContext* context, const ::HIK_SDK_P::ConfParam& request, ::grpc::CompletionQueue* cq) {
  return ::grpc::internal::ClientAsyncResponseReaderFactory< ::HIK_SDK_P::DeviceList>::Create(channel_.get(), cq, rpcmethod_GetDVRConfig_, context, request, true);
}

::grpc::ClientAsyncResponseReader< ::HIK_SDK_P::DeviceList>* HIKSDK::Stub::PrepareAsyncGetDVRConfigRaw(::grpc::ClientContext* context, const ::HIK_SDK_P::ConfParam& request, ::grpc::CompletionQueue* cq) {
  return ::grpc::internal::ClientAsyncResponseReaderFactory< ::HIK_SDK_P::DeviceList>::Create(channel_.get(), cq, rpcmethod_GetDVRConfig_, context, request, false);
}

HIKSDK::Service::Service() {
  AddMethod(new ::grpc::internal::RpcServiceMethod(
      HIKSDK_method_names[0],
      ::grpc::internal::RpcMethod::NORMAL_RPC,
      new ::grpc::internal::RpcMethodHandler< HIKSDK::Service, ::HIK_SDK_P::LoginInfo, ::HIK_SDK_P::ErrCode>(
          std::mem_fn(&HIKSDK::Service::Init), this)));
  AddMethod(new ::grpc::internal::RpcServiceMethod(
      HIKSDK_method_names[1],
      ::grpc::internal::RpcMethod::NORMAL_RPC,
      new ::grpc::internal::RpcMethodHandler< HIKSDK::Service, ::HIK_SDK_P::Empty, ::HIK_SDK_P::ErrCode>(
          std::mem_fn(&HIKSDK::Service::Uninit), this)));
  AddMethod(new ::grpc::internal::RpcServiceMethod(
      HIKSDK_method_names[2],
      ::grpc::internal::RpcMethod::NORMAL_RPC,
      new ::grpc::internal::RpcMethodHandler< HIKSDK::Service, ::HIK_SDK_P::SaveFileParam, ::HIK_SDK_P::ErrCode>(
          std::mem_fn(&HIKSDK::Service::SaveFileByTime), this)));
  AddMethod(new ::grpc::internal::RpcServiceMethod(
      HIKSDK_method_names[3],
      ::grpc::internal::RpcMethod::NORMAL_RPC,
      new ::grpc::internal::RpcMethodHandler< HIKSDK::Service, ::HIK_SDK_P::SaveFileParam1, ::HIK_SDK_P::ErrCode>(
          std::mem_fn(&HIKSDK::Service::SaveFileByName), this)));
  AddMethod(new ::grpc::internal::RpcServiceMethod(
      HIKSDK_method_names[4],
      ::grpc::internal::RpcMethod::NORMAL_RPC,
      new ::grpc::internal::RpcMethodHandler< HIKSDK::Service, ::HIK_SDK_P::FindFileParam, ::HIK_SDK_P::FileData>(
          std::mem_fn(&HIKSDK::Service::FindFileByTime), this)));
  AddMethod(new ::grpc::internal::RpcServiceMethod(
      HIKSDK_method_names[5],
      ::grpc::internal::RpcMethod::NORMAL_RPC,
      new ::grpc::internal::RpcMethodHandler< HIKSDK::Service, ::HIK_SDK_P::ConfParam, ::HIK_SDK_P::DeviceList>(
          std::mem_fn(&HIKSDK::Service::GetDVRConfig), this)));
}

HIKSDK::Service::~Service() {
}

::grpc::Status HIKSDK::Service::Init(::grpc::ServerContext* context, const ::HIK_SDK_P::LoginInfo* request, ::HIK_SDK_P::ErrCode* response) {
  (void) context;
  (void) request;
  (void) response;
  return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
}

::grpc::Status HIKSDK::Service::Uninit(::grpc::ServerContext* context, const ::HIK_SDK_P::Empty* request, ::HIK_SDK_P::ErrCode* response) {
  (void) context;
  (void) request;
  (void) response;
  return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
}

::grpc::Status HIKSDK::Service::SaveFileByTime(::grpc::ServerContext* context, const ::HIK_SDK_P::SaveFileParam* request, ::HIK_SDK_P::ErrCode* response) {
  (void) context;
  (void) request;
  (void) response;
  return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
}

::grpc::Status HIKSDK::Service::SaveFileByName(::grpc::ServerContext* context, const ::HIK_SDK_P::SaveFileParam1* request, ::HIK_SDK_P::ErrCode* response) {
  (void) context;
  (void) request;
  (void) response;
  return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
}

::grpc::Status HIKSDK::Service::FindFileByTime(::grpc::ServerContext* context, const ::HIK_SDK_P::FindFileParam* request, ::HIK_SDK_P::FileData* response) {
  (void) context;
  (void) request;
  (void) response;
  return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
}

::grpc::Status HIKSDK::Service::GetDVRConfig(::grpc::ServerContext* context, const ::HIK_SDK_P::ConfParam* request, ::HIK_SDK_P::DeviceList* response) {
  (void) context;
  (void) request;
  (void) response;
  return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
}


}  // namespace HIK_SDK_P

