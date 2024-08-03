/////////////////////////////////////////////////////////////////////////////////////////////
// Copyright 2024 Garmin International, Inc.
// Licensed under the Flexible and Interoperable Data Transfer (FIT) Protocol License; you
// may not use this file except in compliance with the Flexible and Interoperable Data
// Transfer (FIT) Protocol License.
/////////////////////////////////////////////////////////////////////////////////////////////
// ****WARNING****  This file is auto-generated!  Do NOT edit this file.
// Profile Version = 21.141.0Release
// Tag = production/release/21.141.0-0-g2aa27e1
/////////////////////////////////////////////////////////////////////////////////////////////


#if !defined(FIT_MESG_WITH_EVENT_HPP)
#define FIT_MESG_WITH_EVENT_HPP

#include "fit.hpp"
#include "fit_profile.hpp"

namespace fit
{

class MesgWithEvent
{
public:
    virtual ~MesgWithEvent() {}
    virtual FIT_DATE_TIME GetTimestamp(void) const = 0;
    virtual void SetTimestamp(FIT_DATE_TIME timestamp) = 0;
    virtual FIT_EVENT GetEvent() const = 0;
    virtual void SetEvent(FIT_EVENT event) = 0;
    virtual FIT_EVENT_TYPE GetEventType() const = 0;
    virtual void SetEventType(FIT_EVENT_TYPE type) = 0;
    virtual FIT_UINT8 GetEventGroup() const = 0;
    virtual void SetEventGroup(FIT_UINT8 group) = 0;
};

} // namespace fit

#endif // !defined(FIT_MESG_WITH_EVENT_HPP)
