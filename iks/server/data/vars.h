#ifndef __DATA_VARS_H__
#define __DATA_VARS_H__

#include <string>

#include "sets.h"
#include "params.h"
#include "config.h"
#include "szbase_wrapper.h"
#include "params_updater.h"

#include "utils/signals.h"
#include "utils/exception.h"

typedef boost::signals2::signal<void (const std::string&,const std::string&)> sig_command;
typedef sig_command::slot_type sig_command_slot;

class Vars {
public:
	Vars( );
	virtual ~Vars();

	void from_szarp( const std::string& szarp_base ) throw(file_not_found_error,xml_parse_error);

	void set_szarp_prober_server( const std::string& address , unsigned port );

	void command_request( const std::string& cmd , const std::string& data ) const;

	void response_received( const std::string& cmd , const std::string& data );

	slot_connection on_command_received( const sig_command_slot& slot ) const
	{	return emit_command_received.connect( slot ); }

	slot_connection on_command_response_received( const sig_command_slot& slot ) const
	{	return emit_command_response_received.connect( slot ); }

	const Params& get_params() const
	{	return params; }
	const Sets&   get_sets  () const
	{	return sets; }
	const Config& get_config() const
	{	return config; }
	const SzbaseWrapper* get_szbase() const
	{	return szb_wrapper; }

	Params& get_params()
	{	return params; }
	Sets& get_sets()
	{	return sets; }
	Config& get_config()
	{	return config; }
	ParamsUpdater& get_updater()
	{	return params_updater; }

	bool is_initialized() const
	{	return initialized; }

protected:

	Params params;
	Sets   sets;
	Config config;

	ParamsUpdater params_updater;
	SzbaseWrapper* szb_wrapper;

private:
	bool initialized;

	mutable sig_command emit_command_received;
	mutable sig_command emit_command_response_received;
};

#endif /* __DATA_VARS_H__ */

