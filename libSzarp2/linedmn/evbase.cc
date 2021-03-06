#include "evbase.h"

// necessary includes for evutil_socket_error_to_string
#include <cerrno>
#include <cstring>

#include <cmath>

EventBase::EventBase(PEvBase base)
{
	if (base) {
		m_event_base = base;
		m_priorities_num = -1;	// in libevent 2.0 we have no way of knowing it
	} else {
		// construct a new event base
		const int default_priorities_num = 3;	// priors are 0,1,..,(p_num - 1)
		// the default priority = floor(priorities_num / 2)
		struct event_base* eb = event_base_new();
		event_base_priority_init(eb, default_priorities_num);
		m_event_base = std::shared_ptr<struct event_base>(eb, event_base_free);
		m_priorities_num = default_priorities_num;
	}
}

PEvent EventBase::CreateEvent(evutil_socket_t fd, short what,
	event_callback_fn cb, void *arg) const
{
	struct event *ev = event_new(m_event_base.get(), fd, what, cb, arg);
	if (!ev) {
		EventBaseException ex;
		ex.SetMsg("CreateEvent() failed");
		throw ex;
	}
	PEvent event = std::shared_ptr<struct event>(ev, event_free);
	return event;
}

PEvent EventBase::CreateTimer(event_callback_fn cb, void *arg) const
{
	struct event *ev = evtimer_new(m_event_base.get(), cb, arg);
	if (!ev) {
		EventBaseException ex;
		ex.SetMsg("CreateEvent() failed");
		throw ex;
	}
	PEvent timer = std::shared_ptr<struct event>(ev, event_free);
	return timer;
}

PBufferevent EventBase::CreateBufferevent(evutil_socket_t fd, int options) const
{
	struct bufferevent *ev = 
		bufferevent_socket_new(m_event_base.get(), fd, options);
	if (!ev) {
		EventBaseException ex;
		ex.SetMsg("CreateBufferevent() failed");
		throw ex;
	}
	// bufferevents are created with write on and read off

	PBufferevent pb = std::shared_ptr<struct bufferevent>(ev, bufferevent_free);
	return pb;
}

void EventBase::BuffereventConnect(PBufferevent bev,
	const struct sockaddr_in *sin) const
{
	int res = bufferevent_socket_connect(bev.get(),
		(struct sockaddr*)sin, sizeof(*sin));
	if (res != 0) {
		const int error = EVUTIL_SOCKET_ERROR();	// on Unix same as errno
		EventBaseException ex;
		ex.SetMsg("bufferevent_socket_connect() failed. Error %d: '%s'",
			error, evutil_socket_error_to_string(error));
		throw ex;
	}
}

PEvconnListener EventBase::CreateListenerBind(evconnlistener_cb cb,
	void *ptr, unsigned flags, int backlog,
	const struct sockaddr_in *sin) const
{
	struct evconnlistener *listener = evconnlistener_new_bind(
		m_event_base.get(), cb, ptr, flags, backlog,
		(struct sockaddr*) sin, sizeof(*sin));
	
	if (!listener) {
		const int port = ntohs(sin->sin_port);
		const int error = EVUTIL_SOCKET_ERROR();
		EventBaseException ex;
		ex.SetMsg("evconnlistener_new_bind() failed on port %d. Error %d: '%s'",
			port, error, evutil_socket_error_to_string(error));
		throw ex;
	}
	return std::shared_ptr<struct evconnlistener>(listener, evconnlistener_free);
}

void EventBase::Dispatch()
{
	int res = event_base_dispatch(m_event_base.get());
	if (res != 0) {
		EventBaseException ex;
		ex.SetMsg("event_base_dispatch() failed");
		throw ex;
	}
}

void EventBase::EvtimerDel(PEvent ev) const
{
	int res = evtimer_del(ev.get());
	if (res != 0) {
		EventBaseException ex;
		ex.SetMsg("evtimer_del() failed");
		throw ex;
	}
}

void EventBase::EvtimerAddMs(PEvent ev, ms_time_t ms) const
{
	struct timeval tv = ms2timeval(ms);
	int res = evtimer_add(ev.get(), &tv);
	if (res != 0) {
		EventBaseException ex;
		ex.SetMsg("evtimer_add_ms() failed");
		throw ex;
	}
}

void EventBase::SetTimeoutsMs(PBufferevent bev,
	ms_time_t read_ms, ms_time_t write_ms) const
{
	struct timeval tv_read = ms2timeval(read_ms);
	struct timeval tv_write = ms2timeval(write_ms);
	bufferevent_set_timeouts(bev.get(), &tv_read, &tv_write);
}

void EventBase::SetEventPriority(PEvent ev, const EventPriority priority) const
{
	int pending = event_pending(ev.get(),
		EV_READ | EV_WRITE | EV_SIGNAL | EV_TIMEOUT, NULL);
	if (pending) {
		EventBaseException ex;
		ex.SetMsg("SetEventPriority: cannot set priority on a pending event");
		throw ex;
	}
	const int priority_int = PriorityToInt(priority);
	int res = event_priority_set(ev.get(), priority_int);
	if (res != 0) {
		EventBaseException ex;
		ex.SetMsg("SetEventPriority: setting priority failed");
		throw ex;
	}
}

int EventBase::PriorityToInt(const EventPriority priority) const
{
	// event_base_get_npriorities is not available for libevent2.0 (wheezy)
	if (m_priorities_num < 0) {
		EventBaseException ex;
		ex.SetMsg("PriorityToInt: setting priorities not supported");
		throw ex;
	}
	if (priority == EventPriority::HIGH) {
		return 0;
	} else if (priority == EventPriority::LOW) {
		return m_priorities_num - 1;
	} else {	// MEDIUM
		return std::floor(m_priorities_num / 2);
	}
}
