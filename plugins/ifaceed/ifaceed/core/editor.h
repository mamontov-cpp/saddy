/*! \file   core::Editor.h
	\author HiddenSeeker

	Defines a main interface editor class
 */
#include "../macro.h"

#include "../history/history.h"

#include "../core/qttarget.h"
#include "../core/editorbehaviour.h"
#include "../core/quitreason.h"


#include "../mainpanel.h"

#include "shared.h"
#include "objectxmlwriter.h"

#include <scene.h>
#include <sadmutex.h>
#include <renderer.h>
#include <closure.h>

#include <cli/parser.h>

#include <input/controls.h>

#include <config/sprite2dconfig.h>

#include <hfsm/hfsmmachine.h>
#include <hfsm/hfsmstate.h>
#include <hfsm/hfsmhandler.h>

#include <QObject>
#include <QTimer>
#include <QThread>
#include <QApplication>
#include <QMainWindow>


#pragma once

class ScreenTemplate;
class FontTemplateDatabase;
class SelectedObjectBorder;
class IFaceSharedData;

namespace core
{
class EditorBehaviour;
class Shared;
class SaddyThread;
class Synchronization;

namespace borders
{
class ActiveBorder;
class SelectionBorder;
}

/*! A main editor class, which works as event receiver and data container
 */
class Editor: public QObject
{
Q_OBJECT

friend class core::SaddyThread;
friend class core::QtTarget;
public:
	/*! Constructs editor with empty db
	 */
	Editor();
	/*! Frees memory from db
	 */
	~Editor();
	/*! Inits an editor, loading default data if nothing specified
		\param[in] argc count of arguments
		\param[in] argv arguments
	 */
	virtual void init(int argc,char ** argv);
	/*! Returns editor's window, used by Qt part of application
		\return editor's window
	 */
    MainPanel* panel() const;
	/*! Returns a machine for editor
		\return machine editor
	 */
    sad::hfsm::Machine* machine() const;
    /*! Returns shared state data
        \return shared state data information
     */
    core::Shared* shared() const;
    /*! Returns parsed arguments
        \return parsed arguments
     */
    sad::cli::Parser* parsedArgs() const;
    /*! Returns a history
        \return history
    */
    history::History * history() const;
    /*! Returns main QApplication instance
        \return qt application
     */
    QApplication* app() const;
    /*! A synonym for sad::Renderer::ref()
        \return current renderer
     */
    sad::Renderer* renderer() const;
    /*! Returns synchronization primitive
        \return synchronization primitive
     */
    core::Synchronization* synchronization() const;
	/*! Returns active border, which renders borders of current active object
		\return active border
	 */
	core::borders::ActiveBorder* activeBorder() const;
	/*! Returns selection border, which is used for highlighting current item
		\return selection border
	 */
	core::borders::SelectionBorder* selectionBorder() const;
    /*! Quits an editor
     */
    void quit();
    /*! Emits a signal core::Editor::closureArrived()
        with specified arguments
        \param[in] closure closure signal argument
     */
    void emitClosure(sad::ClosureBasic* closure);
	/*! Cleans up if we were adding stuff before clicking "Add XXX", like "Add label" or "Add sprite"
	 */
	void cleanupBeforeAdding();
    /*!
     * Tests whether editor is in selected state and current selected node equals passed
     * \param[in] node checked node
     * \return whether node is selected
     */
    bool isNodeSelected(sad::SceneNode* node) const;


	/*! Returns a database fwith all of resources
	 */
	FontTemplateDatabase * database();
	/*! Tries to render active object, if any
	 */ 
	void tryRenderActiveObject();
	/*! Returns current resulting in-game screen
		\return result
	 */
	inline ScreenTemplate * result()
	{
		return m_result;
	}
	/*! Shows objects stats for selected object
		Implemented in \\core\\states\\changingselection.h
		\param[in] o object, which must be selected
	 */
	virtual void showObjectStats(AbstractScreenObject * o);
	/*! Tries to select some object
		Implemented in \\core\\states\\changingselection.h
		\param[in] p point
		\param[in] enterSelected whether we should enter selected states if found
	 */
	virtual void trySelectObject(sad::Point2D p, bool enterSelected);
	/*! Returns a scene
		\return used scene
	*/
	inline sad::Scene * scene() { return this->m_scene; }
	/*! Returns a behaviour hash
		\return behaviour hash
	*/
	inline sad::Hash<sad::String, core::EditorBehaviour *> & behaviours() 
	{
		return m_behaviours;
	}
	/*! Erases a current behaviour for work and sets for nothing
	*/
	void eraseBehaviour();

	/*! Deactivates old behaviour and enters new behaviour
		\param[in]  name name of new behaviour
	*/
	virtual void setBehaviour(const sad::String & name);
	/*! Enters a behaviour of editor
		\return editor behaviour
	*/
	core::EditorBehaviour * currentBehaviour();	
	/*! Casts a current behaviour state identified by state to needed
		\param[in] s string name of state
		\return state
	*/
	template<typename T> T * cbStateAs(const sad::String & s) {
		return static_cast<T *>(this->currentBehaviour()->getState(s));
	}
	/*! Returns an icon container
	 */
	sad::Sprite2DConfig & icons();	 	
public slots:
	/*! Called, when Qt Event Loop is started. Used to load default resources and pre-set
		default behaviour
	 */
	void start();
	/*! Undoes history action 
	 */
	void undo();
	/*! Redoes history action
	 */
	void redo();


	/*! Tries erasing object, depending on current object state
	 */
	virtual void tryEraseObject();
	/*! Updates a list from event
	 */
	virtual void submitEvent(const sad::String & eventType, const sad::db::Variant & v);
	/*! Appends a rotation command to a history
		Used for deferred rotation appendance
	 */
	void appendRotationCommand();
	/*! Performs full texture reload from a data
		Uses current parsed data from reload, all object schemes and stuff.
		Reload must be hit only from Qt code, otherwise 
		everything would fail. 
     */
	void reload();
	/*!  Saves a screen template into a file
	 */ 
	void save();
	/*!  Loads an editor data from a file
	 */
	void load();
signals:
	/*! Signal is emitted, when closure is arrived
		\param[in] closure data for closure
     */
	void closureArrived(sad::ClosureBasic * closure);	
protected:
    /*! Target for sending information
     */
    core::QtTarget* m_qttarget;
    /*! Thread for rendering
     */
    core::SaddyThread* m_renderthread;
    /*! A synchronization primitive for
        synchronizing threads
     */
    core::Synchronization* m_synchronization;
    /*! Main window of application
     */
    MainPanel* m_mainwindow;
    /*! Application of qt, which is used
     */
    QApplication* m_qtapp;
    /*! A hierarchical state machine
     */
    sad::hfsm::Machine* m_machine;
    /*! Command line arguments
     */
    sad::cli::Args* m_args;
    /*! A parsed command-line arguments
     */
    sad::cli::Parser* m_parsed_args;
    /*! History of data
     */
    history::History* m_history;
    /*! Describes a behaviour shared data
     */
    core::Shared* m_shared;
    /*! A reason, while editor must be quit
     */
    core::QuitReason  m_quit_reason;
	/*! An active border, which renders current active object
	 */
	core::borders::ActiveBorder* m_active_border;
	/*! A selection border, which is used for higlighting current item
	 */
	core::borders::SelectionBorder* m_selection_border;

    /*! Reports errors to log
        \param[in, out] errors a list of errors
        \param[in] name a name of file, which has been loading
     */
    void reportResourceLoadingErrors(
        sad::Vector<sad::resource::Error *> & errors,
        const sad::String& name
    );
    /*! Initializes conversion table with all conversion table
     */
    void initConversionTable();
    /*! Called, when core::SaddyThread finishes working. Sets
        reason, why editor to quit as initiated by Saddy's renderer
        and calls cleanup actions
     */
    void saddyQuitSlot();




	/*!	Counter for loading all of dbs
	*/
    int m_counter;
	/*! A template database with fonts
	*/
	FontTemplateDatabase * m_db;
	/*! Screen template data
	*/
	ScreenTemplate * m_result;
	/*! A defines editor behaviours
	*/
	sad::Hash<sad::String, core::EditorBehaviour *> m_behaviours;
	/*! A current behaviour
	*/
	sad::String m_current_behaviour;
	/*! Sets a database for templates
		\param[in] db database
	*/
	void setDatabase(FontTemplateDatabase * db);    
protected slots:
    /*! Creates QApplication and window,
        sets up global key handlers and starts event loop,
        calling core::Editor::start(), after event loop is started
     */
	virtual void runQtEventLoop();
    /*! Runs saddy renderer's event loop. Called inside SaddyThread::run,
        sets quit flag,
		when saddy quits working
     */
	virtual void runSaddyEventLoop();
    /*! Сalled, when user closes last Qt window. Sets reason, why editor is quit
        and calls quit actions
     */
    void qtQuitSlot();
    /*! Depending on reasons, why editor is quit closes saddy renderer's window
     *  or panel window
     */
	void onQuitActions();
    /*! Runs a closure. Used by main thread to work
        with user-defined actions.
		\param[in] closure closure data
     */
    virtual void runClosure(sad::ClosureBasic * closure);
private:	
	/*! A scene used for output
	*/
	sad::Scene* m_scene;	
	/*! An icons container
	*/
	sad::Sprite2DConfig m_icons;
};

}
