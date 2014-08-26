#include "editorhistory.h"
#include <3rdparty/format/format.h>
#include <db/load.h>
#include <db/save.h>
#include <log/log.h>

EditorHistory::EditorHistory()
{
	m_current = -1;
}

EditorHistory::~EditorHistory()
{
	this->clear();
}

void EditorHistory::clear()
{
	for (int i=0;i<m_commands.count();i++)
	{
		delete m_commands[i];
	}
	m_commands.clear();
	m_current = -1;
}

void EditorHistory::commit(core::Editor * ob)
{
	SL_SCOPE("EditorHistory::commit");
	SL_DEBUG(
		str(
			fmt::Format("At position {0} with {1} commands")
			<< m_current << m_commands.count()
		)
	);
	if (m_commands.count() && m_current!=m_commands.count()-1)
	{
		SL_DEBUG( 
			str(
				fmt::Format("Commiting command {0}")
				<< m_current + 1
			)
		);
		m_commands[m_current+1]->commit(ob);
		++m_current;
	}
	else
	{
		SL_DEBUG("Nothing to commit");
	}
}

void EditorHistory::rollback(core::Editor * ob)
{
	SL_SCOPE("EditorHistory::rollback");
	SL_DEBUG(
		str(
			fmt::Format("At position {0} with {1} commands")
			<< m_current << m_commands.count()
		)
	);
	if (m_commands.count() && m_current!=-1)
	{
		SL_DEBUG( 
			str(
				fmt::Format("Rolling back command {0}")
				<< m_current
			)
		);
		m_commands[m_current]->rollback(ob);
		--m_current;
	}
	else
	{
		SL_DEBUG("Nothing to rollback");
	}
}

void EditorHistory::add(AbstractCommand * c)
{
	int count = m_commands.count();
	for(int i=m_current+1;i<count;i++)
	{
		delete m_commands[m_current+1];
		m_commands.removeAt(m_current+1);
	}
	m_commands << c;
	++m_current;
}
