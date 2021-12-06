
#include <boost/asio.hpp>

#include <boost/iostreams/stream.hpp>

#include <napi.h>

extern "C" {
  #include <zstd.h>
}

/////////////////

class Compression : public Napi::ObjectWrap<Compression> {

  public:
    static void Init(Napi::Env env, Napi::Object exports);
    Compression(const Napi::CallbackInfo& info);

  private:
    static Napi::Value RecommendedInputBufferSize(const Napi::CallbackInfo& info);
    static Napi::Value RecommendedOutputBufferSize(const Napi::CallbackInfo& info);

    ZSTD_CCtx* const cctx = ZSTD_createCCtx();

    Napi::Value Compress(const Napi::CallbackInfo& info);
    Napi::Value End(const Napi::CallbackInfo& info);
};

void Compression::Init(Napi::Env env, Napi::Object exports) {

    Napi::Function func = DefineClass(env, "Compression", {
        StaticAccessor<&Compression::RecommendedInputBufferSize>("recommendedInputBufferSize"),
        StaticAccessor<&Compression::RecommendedOutputBufferSize>("recommendedOutputBufferSize"),
        InstanceMethod<&Compression::Compress>("compress"),
        InstanceMethod<&Compression::End>("end"),
    });

    Napi::FunctionReference* constructor = new Napi::FunctionReference();
    *constructor = Napi::Persistent(func);
    exports.Set("Compression", func);
}

Compression::Compression(const Napi::CallbackInfo& info) : Napi::ObjectWrap<Compression>(info) {
    ZSTD_CCtx_setParameter(cctx, ZSTD_c_strategy, ZSTD_btultra2);
    ZSTD_CCtx_setParameter(cctx, ZSTD_c_compressionLevel, 22);
}

Napi::Value Compression::RecommendedInputBufferSize(const Napi::CallbackInfo& info) {
    auto env = info.Env();
    return Napi::Number::New(env, ZSTD_CStreamInSize());
}

Napi::Value Compression::RecommendedOutputBufferSize(const Napi::CallbackInfo& info) {
    auto env = info.Env();
    return Napi::Number::New(env, ZSTD_CStreamOutSize());
}

Napi::Value compress(const Napi::CallbackInfo& info, ZSTD_CCtx* const cctx, ZSTD_EndDirective directive) {

    auto env = info.Env();

    auto src = info[0].As<Napi::Object>();
    auto dst = info[1].As<Napi::Object>();

    auto srcBuffer  = src.Get("buffer").As<Napi::Buffer<uint8_t>>();
    size_t srcStart = src.Get("start").ToNumber().Int32Value();
    size_t srcEnd   = src.Get("end").ToNumber().Int32Value();
    size_t srcPos   = src.Get("pos").ToNumber().Int32Value();

    auto dstBuffer  = dst.Get("buffer").As<Napi::Buffer<uint8_t>>();
    size_t dstStart = dst.Get("start").ToNumber().Int32Value();
    size_t dstEnd   = dst.Get("end").ToNumber().Int32Value();
    size_t dstPos   = dst.Get("pos").ToNumber().Int32Value();

    ZSTD_inBuffer  input  = { srcBuffer.Data() + srcStart, srcEnd - srcStart, srcPos };
    ZSTD_outBuffer output = { dstBuffer.Data() + dstStart, dstEnd - dstStart, dstPos };

    auto ret = ZSTD_compressStream2(cctx, &output, &input, directive);

    src.Set("pos", Napi::Number::New(env, input.pos));
    dst.Set("pos", Napi::Number::New(env, output.pos));

    return Napi::Number::New(env, ret);
}

Napi::Value Compression::Compress(const Napi::CallbackInfo& info) {
    ZSTD_CCtx* const cctx = this->cctx;
    return compress(info, cctx, ZSTD_e_continue);
}

Napi::Value Compression::End(const Napi::CallbackInfo& info) {
    ZSTD_CCtx* const cctx = this->cctx;
    return compress(info, cctx, ZSTD_e_end);
}

////////////////////////////////////

class Decompression : public Napi::ObjectWrap<Decompression> {

  public:
    static void Init(Napi::Env env, Napi::Object exports);
    Decompression(const Napi::CallbackInfo& info);

  private:
    static Napi::Value RecommendedInputBufferSize(const Napi::CallbackInfo& info);
    static Napi::Value RecommendedOutputBufferSize(const Napi::CallbackInfo& info);

    ZSTD_DCtx* const dctx = ZSTD_createDCtx();

    Napi::Value Decompress(const Napi::CallbackInfo& info);
};

void Decompression::Init(Napi::Env env, Napi::Object exports) {

    Napi::Function func = DefineClass(env, "Decompression", {
        StaticAccessor<&Decompression::RecommendedInputBufferSize>("recommendedInputBufferSize"),
        StaticAccessor<&Decompression::RecommendedOutputBufferSize>("recommendedOutputBufferSize"),
        InstanceMethod<&Decompression::Decompress>("decompress"),
    });

    Napi::FunctionReference* constructor = new Napi::FunctionReference();
    *constructor = Napi::Persistent(func);
    exports.Set("Decompression", func);
}

Decompression::Decompression(const Napi::CallbackInfo& info) : Napi::ObjectWrap<Decompression>(info) { }

Napi::Value Decompression::RecommendedInputBufferSize(const Napi::CallbackInfo& info) {
    auto env = info.Env();
    return Napi::Number::New(env, ZSTD_DStreamInSize());
}

Napi::Value Decompression::RecommendedOutputBufferSize(const Napi::CallbackInfo& info) {
    auto env = info.Env();
    return Napi::Number::New(env, ZSTD_DStreamOutSize());
}

Napi::Value Decompression::Decompress(const Napi::CallbackInfo& info) {

    auto env = info.Env();

    auto src = info[0].As<Napi::Object>();
    auto dst = info[1].As<Napi::Object>();

    auto srcBuffer  = src.Get("buffer").As<Napi::Buffer<uint8_t>>();
    size_t srcStart = src.Get("start").ToNumber().Int32Value();
    size_t srcEnd   = src.Get("end").ToNumber().Int32Value();
    size_t srcPos   = src.Get("pos").ToNumber().Int32Value();

    auto dstBuffer  = dst.Get("buffer").As<Napi::Buffer<uint8_t>>();
    size_t dstStart = dst.Get("start").ToNumber().Int32Value();
    size_t dstEnd   = dst.Get("end").ToNumber().Int32Value();
    size_t dstPos   = dst.Get("pos").ToNumber().Int32Value();

    ZSTD_DCtx* const dctx = this->dctx;

    ZSTD_inBuffer  input  = { srcBuffer.Data() + srcStart, srcEnd - srcStart, srcPos };
    ZSTD_outBuffer output = { dstBuffer.Data() + dstStart, dstEnd - dstStart, dstPos };

    auto ret = ZSTD_decompressStream(dctx, &output, &input);

    src.Set("pos", Napi::Number::New(env, input.pos));
    dst.Set("pos", Napi::Number::New(env, output.pos));

    return Napi::Number::New(env, ret);
}

////////////////////////////////////

Napi::Object Init(Napi::Env env, Napi::Object exports) {
  Compression::Init(env, exports);
  Decompression::Init(env, exports);
  return exports;
}

NODE_API_MODULE(NODE_GYP_MODULE_NAME, Init)
