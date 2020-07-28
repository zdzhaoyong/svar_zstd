#define ZSTD_STATIC_LINKING_ONLY   /* ZSTD_magicNumber, ZSTD_frameHeaderSize_max */
#include "zstd.h"
#include <Svar/Svar.h>

sv::SvarBuffer compress(sv::SvarBuffer in,int compressionLevel){
    sv::SvarBuffer dst(ZSTD_compressBound(in.size()));
    size_t size=ZSTD_compress(dst._ptr,dst.size(),in._ptr,in.size(),compressionLevel);

    return sv::SvarBuffer(dst._ptr,size,dst._holder);
}

sv::SvarBuffer decompress(sv::SvarBuffer in){
    ZSTD_DStream*  ress=ZSTD_createDStream();
    sv::SvarBuffer out(ZSTD_CStreamOutSize());

    ZSTD_outBuffer outBuf={out._ptr,out.size(),0};
    ZSTD_inBuffer inBuff ={in._ptr,in.size(),0};

    std::vector<sv::SvarBuffer> bufs;
    size_t         totalBytes=0;
    while(true){
        size_t const readSizeHint = ZSTD_decompressStream(ress,&outBuf,&inBuff);
        if (ZSTD_isError(readSizeHint)) {
            return sv::SvarBuffer(0);
        }

        bufs.push_back(sv::SvarBuffer(outBuf.dst,outBuf.pos).clone());
        totalBytes+=outBuf.pos;
        outBuf.pos=0;

        if (readSizeHint == 0) break;   /* end of frame */
    }

    ZSTD_freeDStream(ress);
    if(bufs.size()==1)
        return bufs.front();

    sv::SvarBuffer buf(totalBytes);
    size_t cached=0;
    for(sv::SvarBuffer b:bufs)
    {
        memcpy(buf._ptr+cached,b._ptr,b.size());
        cached+=b.size();
    }

    return buf;
}


int main(int argc,char** argv){
    svar.parseMain(argc,argv);

    std::string in=svar.arg<std::string>("in","","The input filepath");
    std::string out=svar.arg<std::string>("out","","The output filepath");
    bool        c=svar.arg("c",false,"Compress");
    int         level=svar.arg("l",4,"Compress level");

    if(svar.get("help",false)||in.empty()||out.empty()) return svar.help();

    sv::SvarBuffer buf=sv::SvarBuffer::load(in);
    sv::SvarBuffer outBuf(0);

    if(c)
        outBuf=compress(buf,level);
    else
        outBuf=decompress(buf);

    outBuf.save(out);

    return 0;
}

REGISTER_SVAR_MODULE(zstd){
    svar["compress"]  =compress;
    svar["decompress"]=decompress;
}
