/**
 * /author Pawel Ptasznik
 */

#include "AsyncFrameListener.hpp"

AsyncFrameListener::AsyncFrameListener(std::shared_ptr<INetworkClient> networkClient): logger_("AsyncFrameListener")
  , networkClient_(networkClient)
  , cb_(nullptr)
  , asyncTask_()
{
  logger_->debug("CTor");
}

AsyncFrameListener::~AsyncFrameListener()
{
  stop();
}

bool AsyncFrameListener::start()
{
  return false;
}

bool AsyncFrameListener::stop()
{
  return false;
}
