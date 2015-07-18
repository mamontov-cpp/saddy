#include "animations/animationsanimations.h"

// ========================= PUBLIC METHODS =========================

sad::animations::Animations::Animations()
{
	
}

sad::animations::Animations::~Animations()
{
	this->clearNow();
}

sad::animations::SavedObjectStateCache& sad::animations::Animations::cache()
{
	return m_cache;
}

// ========================= PROTECTED METHODS =========================

void sad::animations::Animations::_process()
{
	performQueuedActions();
	lockChanges();

	for(size_t i = 0; i < m_list.size(); i++)
	{
		sad::animations::Process * p = m_list[i];
        p->process(this);
		if (p->finished())
		{
			p->removedFromPipeline();
			m_list.removeAt(i);
			--i;
		}
	}

	unlockChanges();
	performQueuedActions();
}

void sad::animations::Animations::addNow(sad::animations::Process* o)
{
	if (o)
	{
		o->addedToPipeline();
		m_list << o;
	}
}

void sad::animations::Animations::removeNow(sad::animations::Process* o)
{
	if (o)
	{
        sad::Vector<sad::animations::Process*>::iterator it = std::find(
            m_list.begin(), 
            m_list.end(),
            o
        );
        if (it != m_list.end())
        {
            o->cancel(this);
		    o->removedFromPipeline();
		    m_list.erase(it);
        }
	}
}

void sad::animations::Animations::clearNow()
{
	for(size_t i = 0; i < m_list.size(); i++)
	{
        m_list[i]->cancel(this);		    
		m_list[i]->removedFromPipeline();
	}
	m_list.clear();
}
