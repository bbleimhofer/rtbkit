/* sharded_event_matcher.h                                 -*- C++ -*-
   Rémi Attab (remi.attab@gmail.com), 24 Apr 2014
   FreeBSD-style copyright and disclaimer apply

   In-memory sharding of the event matcher.
*/

#pragma once

#include "event_matcher.h"
#include "simple_event_matcher.h"
#include "rtbkit/common/auction_events.h"
#include "soa/service/message_loop.h"
#include "soa/service/service_base.h"
#include "soa/service/logs.h"
#include "soa/service/typed_message_channel.h"

namespace RTBKIT {

/******************************************************************************/
/* SHARDED EVENT MATCHER                                                      */
/******************************************************************************/

struct ShardedEventMatcher : public EventMatcher, public MessageLoop
{

    ShardedEventMatcher(std::string prefix, std::shared_ptr<EventService> events);
    ShardedEventMatcher(std::string prefix, std::shared_ptr<ServiceProxies> proxies);

    void init(size_t shards);
    void start();
    void shutdown();

    virtual void setBanker(const std::shared_ptr<Banker> & newBanker);
    virtual void setWinTimeout(const float & timeOut);
    virtual void setAuctionTimeout(const float & timeOut);


    /************************************************************************/
    /* EVENT MATCHING                                                       */
    /************************************************************************/

    /** Handle a new auction that came in. */
    virtual void doAuction(std::shared_ptr<SubmittedAuctionEvent> event);

    /** Handle a post-auction event that came in. */
    virtual void doEvent(std::shared_ptr<PostAuctionEvent> event);

    /** Periodic auction expiry. */
    virtual void checkExpiredAuctions() {}

private:

    struct Shard : public MessageLoop
    {
        Shard(std::string prefix, std::shared_ptr<EventService> events);
        void init(ShardedEventMatcher* parent);

        SimpleEventMatcher matcher;
        TypedMessageSink<std::shared_ptr<SubmittedAuctionEvent> > auctions;
        TypedMessageSink<std::shared_ptr<PostAuctionEvent> > events;
    };

    std::vector< std::unique_ptr<Shard> > shards;
    Shard& shard(const Id& auctionId);

    TypedMessageSink<std::shared_ptr<MatchedWinLoss> > matchedWinLossEvents;
    TypedMessageSink<std::shared_ptr<MatchedCampaignEvent> > matchedCampaignEvents;
    TypedMessageSink<std::shared_ptr<UnmatchedEvent> > unmatchedEvents;
    TypedMessageSink<std::shared_ptr<PostAuctionErrorEvent> > errorEvents;

    static Logging::Category print;
    static Logging::Category error;
    static Logging::Category trace;
};


} // RTBKIT
