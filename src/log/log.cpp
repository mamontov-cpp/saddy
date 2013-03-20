#include "../../include/log/log.h"

static const char * logmesgs[6] = {
	"FATAL",
	"CRITICAL",
	"WARNING",
	"MESSAGE",
	"DEBUG",
	"USER"
};

const char * sad::log::Message::spriority() const
{
	if (m_priority != sad::log::USER || m_user_priority.length() == 0)
	{
		return logmesgs[(int)m_priority];
	}
	return m_user_priority.data();
}


const char * sad::log::Message::stime() const
{
	return asctime(localtime(&m_time));
}

sad::log::Message::Message(const hst::string & message,
					   	   sad::log::Priority priority,
						   const char * file,
						   int line ,
						   const hst::string & subsystem ,
						   const hst::string & user_priority 
					      )
{
	m_message = message;
	m_priority = priority;
	m_file = file;
	m_line = line;
	m_subsystem = subsystem;
	m_user_priority = user_priority;
	m_time = ::time(NULL);
}


hst::string sad::log::Message::fileline()
{
	if (!m_file)
		return hst::string();
	hst::string o;
	char ch = '/';
#ifndef _WIN32
	ch = '//';
#endif	
	o =  (strrchr(m_file, ch) ? strrchr(m_file, ch) + 1 : m_file);
	o << ", ";
	o << hst::string::number(m_line);
	return o;
}

hst::string sad::log::StringCaster<hst::string>::cast(const hst::string & string)
{
	return string;
}


hst::string sad::log::StringCaster<std::string>::cast(const std::string & string)
{
	return string.c_str();
}


sad::log::Target::~Target()
{

}

sad::Log::~Log()
{
	for(int i = 0; i < m_targets.count(); i++)
	{
		delete m_targets[i];
	}
}

void sad::Log::broadcast(const sad::log::Message & m)
{
	for(int i = 0; i < m_targets.count(); i++)
	{
		m_targets[i]->receive(m);
	}
}

hst::string sad::Log::subsystem()
{
	if (m_actions_stack.count() != 0)
	{
		return m_actions_stack[m_actions_stack.count() - 1];
	}
	return "";
}


void sad::Log::createAndBroadcast(const hst::string & mesg, 
				            	  sad::log::Priority priority,
						          const char * file , 
								  int line ,
								  const hst::string & upriority)
{
	sad::log::Message * m = new sad::log::Message(
		mesg,
		priority,
		file,
		line ,
		this->subsystem(),
		upriority
	);
	broadcast(*m);
	delete m;
}

sad::Log & sad::Log::addTarget(sad::log::Target * t)
{
	m_targets << t;
	return *this;
}

sad::Log & sad::Log::removeTarget(sad::log::Target * t)
{
	m_targets.remove(t);
	return *this;
}


