#include "idlestate.h"
#include "../ifaceeditor.h"
#include <templates/closure.h>
#include "../../editorcore/editorbehaviour.h"
#include "../../editorcore/editorbehaviourshareddata.h"
#include "../../objects/screentemplate.h"
#include <marshal/serializableobject.h>


void IdleState::onMouseDown(const sad::Event & ev)
{
	if (ev.key == MOUSE_BUTTON_LEFT) {
		IFaceEditor * ed = this->editor();
		hPointF p(ev.x, ev.y);
		CLOSURE
		CLOSURE_DATA( IFaceEditor * e; hPointF m_p; )
		CLOSURE_CODE( this->e->trySelectObject(m_p, true); )
		INITCLOSURE( CLSET(e, ed); CLSET(m_p, p) );
		SUBMITCLOSURE( ed->emitClosure );
	}
}

void IdleState::enter()
{
	IFaceEditor * ed = this->editor();
	ed->shdata()->setSelectedObject(NULL); // Disable borders
	CLOSURE
	CLOSURE_DATA( IFaceEditor * e; )
	CLOSURE_CODE( this->e->highlightState("Idle"); )
	INITCLOSURE( CLSET(e, ed);  );
	SUBMITCLOSURE( ed->emitClosure );
}
