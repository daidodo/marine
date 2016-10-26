#include <logger.hh>
#include <tools/memory.hh>
#include "impl/engine_impl.hh"
#include "engine.hh"

using namespace marine;

NS_HSHA_BEGIN

CReaperEngine::CReaperEngine()
    : impl_(tools::New<NS_HSHA_IMPL::CHshaEngineImpl>())
{}

CReaperEngine::~CReaperEngine()
{
    if(impl_){
        tools::Delete(impl_);
        impl_ = NULL;
    }
}

bool CReaperEngine::addTcpServer(const __SockAddr & hostAddr, CServerCallback * callback)
{
    if(!impl_)
        return false;
    return impl_->addTcpServer(hostAddr, callback);
}

bool CReaperEngine::addTcpClient(const __SockAddr & peerAddr, CServerCallback * callback)
{
    if(!impl_)
        return false;
    return impl_->addTcpClient(peerAddr, callback);
}

bool CReaperEngine::addTcpClient(const __SockAddr & hostAddr, const __SockAddr & peerAddr, CServerCallback * callback)
{
    if(!impl_)
        return false;
    return impl_->addTcpClient(hostAddr, peerAddr, callback);
}

bool CReaperEngine::addUdp(const __SockAddr & hostAddr, const __SockAddr & peerAddr, CServerCallback * callback)
{
    if(!impl_)
        return false;
    return impl_->addUdp(hostAddr, peerAddr, callback);
}

bool CReaperEngine::run(const CReaperParams & params)
{
    if(!impl_)
        return false;
    return impl_->run(params);
}

void CReaperEngine::stop()
{
    if(impl_)
        impl_->stop();
}

bool CReaperEngine::stopped() const volatile
{
    if(!impl_)
        return true;
    return impl_->stopped();
}

NS_HSHA_END
