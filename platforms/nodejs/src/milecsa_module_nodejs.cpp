//
// Created by lotus mile on 17/10/2018.
//

#include <functional>
#include <optional>
#include "mile_crypto.h"
#include "milecsa_module_nodejs.hpp"

namespace milecsa::nodejs {


    milecsa::ErrorHandler error_handler = [](milecsa::result code, std::string error){
        Nan::ThrowError(Nan::New("milecsa error: " + error).ToLocalChecked());
    };

    ///
    /// Transaction
    ///
    NAN_METHOD(detail::Transaction::New) {
            detail::Transaction* t = new detail::Transaction();
            t->Wrap(info.Holder());
            info.GetReturnValue().Set(info.Holder());
    }

    void __transaction(const Nan::FunctionCallbackInfo<v8::Value>& info, int method) {

        int index = -1;

        v8::Local<v8::Object> jsonObj = info[0]->ToObject();

        v8::Local<v8::String> pk_name = Nan::New("public_key").ToLocalChecked();
        v8::Local<v8::String> pvk_name = Nan::New("private_key").ToLocalChecked();

        std::string public_key;
        std::string private_key;

        std::string dstWalletPublicKey;
        std::string blockId;
        std::string transactionId;

        unsigned short assetCode ;
        float amount;
        float fee;

        std::string desc;

        if (!info[++index]->IsObject()){
            return Nan::ThrowError(Nan::New(
                    ErrorFormat("Transaction - expected argument %i wallet pair",index))
                                           .ToLocalChecked());
        }

        jsonObj = info[index]->ToObject();

        pk_name = Nan::New("public_key").ToLocalChecked();
        pvk_name = Nan::New("private_key").ToLocalChecked();

        if (Nan::HasOwnProperty(jsonObj, pk_name).FromJust()) {
            auto  pkValue = Nan::Get(jsonObj, pk_name).ToLocalChecked();
            public_key = std::string(*Nan::Utf8String(pkValue->ToString()));
        }
        else {
            Nan::ThrowError(Nan::New("milecsa error: Transaction - pair error").ToLocalChecked());
        }

        if (method == 0) {
            if (!info[++index]->IsString()) {
                return Nan::ThrowError(Nan::New(
                        ErrorFormat("Transaction - expected argument %i destination public key", index).c_str())
                                               .ToLocalChecked());
            }

            dstWalletPublicKey = *v8::String::Utf8Value(v8::Local<v8::String>::Cast(info[index]));
        }

        if (!info[++index]->IsString()){
            return Nan::ThrowError(Nan::New(
                    ErrorFormat("Transaction - expected argument %i block id",index).c_str())
                                           .ToLocalChecked());
        }
        blockId = *v8::String::Utf8Value(v8::Local<v8::String>::Cast(info[index]));

        if (!info[++index]->IsString()){
            return Nan::ThrowError(Nan::New(
                    ErrorFormat("Transaction - expected argument %i trx id",index).c_str())
                                           .ToLocalChecked());
        }
        transactionId = *v8::String::Utf8Value(v8::Local<v8::String>::Cast(info[index]));

        if (!info[++index]->IsNumber()){
            return Nan::ThrowError(Nan::New(
                    ErrorFormat("Transaction - expected argument %i asset code",index).c_str())
                                           .ToLocalChecked());
        }
        assetCode = info[index]->NumberValue();

        if (method == 0) {
            if (!info[++index]->IsNumber()) {
                return Nan::ThrowError(Nan::New(
                        ErrorFormat("Transaction - expected argument %i amount", index).c_str())
                                               .ToLocalChecked());
            }
            amount = info[index]->NumberValue();
        }

        if (!info[++index]->IsNumber()){
            return Nan::ThrowError(Nan::New(
                    ErrorFormat("Transaction - expected argument %i fee",index).c_str())
                                           .ToLocalChecked());
        }
        fee = info[index]->NumberValue();

        if (method == 0) {
            if (!info[++index]->IsString()) {
                return Nan::ThrowError(Nan::New(
                        ErrorFormat("Transaction - expected argument %i description", index))
                                               .ToLocalChecked());
            }
            desc = *v8::String::Utf8Value(v8::Local<v8::String>::Cast(info[index]));
        }


        if (Nan::HasOwnProperty(jsonObj, pvk_name).FromJust()) {
            auto  pkValue = Nan::Get(jsonObj, pvk_name).ToLocalChecked();
            private_key = std::string(*Nan::Utf8String(pkValue->ToString()));
        }
        else {
            Nan::ThrowError(Nan::New("milecsa error: Transaction - pair error").ToLocalChecked());
        }

        uint256_t bid;
        uint64_t tid;
        StringToUInt256(blockId, bid, false);
        StringToUInt64(transactionId, tid, false);

        auto p = milecsa::keys::Pair::FromPrivateKey(private_key, error_handler);
        if (!p){
            return;
        }

        using transfer = milecsa::transaction::JsonTransfer;
        using emission = milecsa::transaction::JsonEmission;

        milecsa::transaction::JsonRequest request;

        if (method == 0)
            request = transfer::CreateRequest(
                    *p,
                    dstWalletPublicKey,
                    bid,
                    tid,
                    milecsa::assets::TokenFromCode(assetCode),
                    amount,
                    fee,
                    desc,
                    error_handler);
        else
            request = emission::CreateRequest(
                    *p,
                    bid,
                    tid,
                    milecsa::assets::TokenFromCode(assetCode),
                    fee,
                    error_handler);


        if (request) {

            if (auto trx = request->get_body()) {
                v8::Local<v8::String> json_string = Nan::New(trx->dump()).ToLocalChecked();

                Nan::JSON NanJSON;
                Nan::MaybeLocal<v8::Value> result = NanJSON.Parse(json_string);
                if (!result.IsEmpty()) {
                    v8::Local<v8::Value> val = result.ToLocalChecked();
                    info.GetReturnValue().Set(val);
                }
            }
            else {
                Nan::ThrowError(Nan::New("milecsa error: Transaction - singing error").ToLocalChecked());
            }
        }
    }

    NAN_METHOD(detail::Transaction::Transfer) {
            __transaction(info,0);
    }

    NAN_METHOD(detail::Transaction::Emission) {
            __transaction(info,1);
    }

    ///
    /// Module
    ///
    Nan::Persistent<v8::FunctionTemplate> Module::constructor;

    NAN_MODULE_INIT(Module::Init) {
            v8::Local<v8::FunctionTemplate> ctor = Nan::New<v8::FunctionTemplate>(Module::New);
            constructor.Reset(ctor);
            ctor->InstanceTemplate()->SetInternalFieldCount(1);
            ctor->SetClassName(Nan::New("Module").ToLocalChecked());

            Nan::SetAccessor(ctor->InstanceTemplate(), Nan::New("Pair").ToLocalChecked(), Module::HandleGetters);

            Nan::SetPrototypeMethod(ctor, "Transaction", Transaction);

            target->Set(Nan::New("Module").ToLocalChecked(), ctor->GetFunction());
    }

    NAN_METHOD(Module::New) {
            Module* module = new Module();
            module->Wrap(info.Holder());
            info.GetReturnValue().Set(info.Holder());
    }

    NAN_METHOD(Module::Transaction) {
            const int argc = 1;
            v8::Local<v8::Value> argv[argc] = {info[0]};
            v8::Local<v8::Function> cons = Nan::New(detail::Transaction::constructor());
            info.GetReturnValue().Set(Nan::NewInstance(cons, argc, argv).ToLocalChecked());
    }

    static inline void __update_key(const std::optional<milecsa::keys::Pair> &pair, const Nan::FunctionCallbackInfo<v8::Value>& info){
        v8::Local<v8::Object> object = Nan::New<v8::Object>();

        v8::Local<v8::String> pk_name = Nan::New("public_key").ToLocalChecked();
        v8::Local<v8::Value> pk_value = Nan::New(pair->get_public_key().encode()).ToLocalChecked();

        v8::Local<v8::String> pvk_name = Nan::New("private_key").ToLocalChecked();
        v8::Local<v8::Value> pvk_value = Nan::New(pair->get_private_key().encode()).ToLocalChecked();

        Nan::Set(object, pk_name, pk_value);
        Nan::Set(object, pvk_name, pvk_value);

        info.GetReturnValue().Set(object);
    }

    void Random(const Nan::FunctionCallbackInfo<v8::Value>& info) {

        if (auto pair = milecsa::keys::Pair::Random(error_handler)){
            __update_key(pair,info);
        }
    }

    static inline bool __check_one_args(const Nan::FunctionCallbackInfo<v8::Value>& info, const std::string &name, const std::string &what){
        std::string prefix = "milecsa error: " + name + " - expected ";
        if(info.Length() != 1) {
            prefix += what;
            Nan::ThrowError(Nan::New(prefix).ToLocalChecked());
            return false;
        }

        if(!info[0]->IsString()) {
            prefix += "argument string";
            Nan::ThrowError(Nan::New(prefix).ToLocalChecked());
            return false;
        }

        return true;
    }

    void WithSecret(const Nan::FunctionCallbackInfo<v8::Value>& info) {

        if(!__check_one_args(info,"WithSecret","secret phrase"))
            return;

        v8::String::Utf8Value phrase(v8::Local<v8::String>::Cast(info[0]));

        if (auto pair = milecsa::keys::Pair::WithSecret(*phrase, error_handler)){
            __update_key(pair,info);
        }
    }

    void FromPrivateKey(const Nan::FunctionCallbackInfo<v8::Value>& info) {

        if(!__check_one_args(info,"FromPrivateKey","private key"))
            return;

        v8::String::Utf8Value pvk(v8::Local<v8::String>::Cast(info[0]));

        if (auto pair = milecsa::keys::Pair::FromPrivateKey(*pvk, error_handler)){
            __update_key(pair,info);
        }
    }

    void ValidatePublicKey(const Nan::FunctionCallbackInfo<v8::Value>& info) {

        if(!__check_one_args(info,"ValidatePublicKey","public key"))
            return;

        v8::String::Utf8Value pk(v8::Local<v8::String>::Cast(info[0]));

        if (milecsa::keys::Pair::ValidatePublicKey(*pk, error_handler)){
            info.GetReturnValue().Set(Nan::True());
        } else{
            info.GetReturnValue().Set(Nan::False());
        }
    }

    void ValidatePrivateKey(const Nan::FunctionCallbackInfo<v8::Value>& info) {

        if(!__check_one_args(info,"ValidatePrivateKey","private key"))
            return;

        v8::String::Utf8Value pvk(v8::Local<v8::String>::Cast(info[0]));

        if (milecsa::keys::Pair::ValidatePrivateKey(*pvk, error_handler)){
            info.GetReturnValue().Set(Nan::True());
        } else{
            info.GetReturnValue().Set(Nan::False());
        }
    }

    static inline void __set_function(v8::Local<v8::Object> &object, const std::string name, const v8::Local<v8::FunctionTemplate> &tpl){
        v8::Local<v8::String> key = Nan::New(name).ToLocalChecked();
        v8::Local<v8::Function> fn = tpl->GetFunction();
        fn->SetName(Nan::New(name.c_str()).ToLocalChecked());
        Nan::Set(object, key, fn);
    }

    NAN_GETTER(Module::HandleGetters) {
            Module* self = Nan::ObjectWrap::Unwrap<Module>(info.This());

            std::string propertyName = std::string(*Nan::Utf8String(property));
            if (propertyName == "Pair") {

                v8::Local<v8::Object> object = Nan::New<v8::Object>();

                __set_function(object, "Random", Nan::New<v8::FunctionTemplate>(Random));
                __set_function(object, "WithSecret", Nan::New<v8::FunctionTemplate>(WithSecret));
                __set_function(object, "FromPrivateKey", Nan::New<v8::FunctionTemplate>(FromPrivateKey));
                __set_function(object, "ValidatePublicKey", Nan::New<v8::FunctionTemplate>(ValidatePublicKey));
                __set_function(object, "ValidatePrivateKey", Nan::New<v8::FunctionTemplate>(ValidatePrivateKey));

                info.GetReturnValue().Set(object);

            }
            else {

                info.GetReturnValue().Set(Nan::Undefined());
            }
    }
}