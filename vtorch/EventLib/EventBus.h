#pragma once

#include <eventbus/bus.h>
#include <eventbus/event.h>

#include "Events.h"

namespace event
{
    // Event bus for core functions
    using SystemBus =
        eventbus::EventBus<eventbus::Synchronous, eventbus::UnboundedQueue,
                           eventbus::BlockProducer, events::WindowResize>;

    using Subscription = eventbus::Subscription;

} // namespace event