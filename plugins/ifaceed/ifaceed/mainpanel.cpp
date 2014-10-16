#include "mainpanel.h"
#include "blockedclosuremethodcall.h"
#include "reloadfilelist.h"

#include "core/editor.h"
#include "core/shared.h"
#include "core/selection.h"

#include "history/database/newproperty.h"

#include "history/scenes/scenesadd.h"
#include "history/scenes/scenesremove.h"
#include "history/scenes/scenesclear.h"
#include "history/scenes/sceneschangename.h"
#include "history/scenes/sceneslayerswap.h"

#include "history/scenenodes/scenenodeslayerswap.h"

#include "gui/scenenodeactions.h"
#include "gui/labelactions.h"
#include "gui/sprite2dactions.h"
#include "gui/customobjectactions.h"
#include "gui/wayactions.h"
#include "gui/updateelement.h"
#include "gui/renderways.h"

#include <geometry2d.h>
#include <keymouseconditions.h>
#include <keycodes.h>

#include <p2d/vector.h>
#include <p2d/point.h>

#include <db/save.h>
#include <db/load.h>
#include <db/dbdatabase.h>
#include <db/dbtable.h>
#include <db/dbstoredproperty.h>
#include <db/dbpopulatescenesfromdatabase.h>

#include <resource/tree.h>
#include <freetype/font.h>

#include <QMessageBox>
#include <QDialog>
#include <QTimer>
#include <QFontDatabase>
#include <QItemDelegate>
#include <QPainter>
#include <QColorDialog>
#include <QInputDialog>
#include <QFileDialog>
#include <QLineEdit>
#include <QTimer>
#include <QListWidget>
#include <QListWidgetItem>
#include <QVariant>

#include <cstdio>

Q_DECLARE_METATYPE(sad::Scene*)
Q_DECLARE_METATYPE(sad::SceneNode*)
Q_DECLARE_METATYPE(sad::p2d::app::Way*)

//====================  PUBLIC METHODS HERE ====================

MainPanel::MainPanel(QWidget *parent, Qt::WFlags flags)
	: QMainWindow(parent, flags), m_selfchanged(false)
{
	ui.setupUi(this);

    this->fillDatabasePropertyTypesCombo();

	ui.tabTypes->setCurrentIndex(0);

    ui.twDatabaseProperties->setColumnCount(3);
    const int header_padding = 12;  // 12 is  a padding for header
	double width = ui.twDatabaseProperties->width() - ui.twDatabaseProperties->verticalHeader()->width();
    ui.twDatabaseProperties->setColumnWidth(2, width / 6 - header_padding);
	ui.twDatabaseProperties->setColumnWidth(1, width / 2); 
	ui.twDatabaseProperties->horizontalHeader()->hide();


    width = ui.twCustomObjectProperties->width() - ui.twCustomObjectProperties->verticalHeader()->width();
	ui.twCustomObjectProperties->setColumnCount(2);
    ui.twCustomObjectProperties->setColumnWidth(0, width / 2);
    ui.twCustomObjectProperties->setColumnWidth(1, width / 2 - header_padding * 3.5);
    ui.twCustomObjectProperties->horizontalHeader()->hide();

    ui.txtLabelText->setPlainText("Test");

	m_scene_node_actions = new gui::SceneNodeActions();
	m_scene_node_actions->setPanel(this);

	m_label_actions = new gui::LabelActions();
	m_label_actions->setPanel(this);

	m_sprite2d_actions = new gui::Sprite2DActions();
	m_sprite2d_actions->setPanel(this);

	m_custom_object_actions = new gui::CustomObjectActions();
	m_custom_object_actions->setPanel(this);

	m_way_actions = new gui::WayActions();
	m_way_actions->setPanel(this);
}


MainPanel::~MainPanel()
{
	delete m_label_actions;
	delete m_sprite2d_actions;
	delete m_scene_node_actions;
	delete m_custom_object_actions;
	delete m_way_actions;
	for(sad::PtrHash<sad::String, gui::table::Delegate>::iterator it = m_property_delegates.begin();
		it != m_property_delegates.end();
		++it)
	{
		it.value()->delRef();
	}
	m_property_delegates.clear();
}

void MainPanel::toggleEditingButtons(bool enabled)
{
	const int affectedpushbuttonscount = 18;
	QPushButton* affectedpushbuttons[affectedpushbuttonscount] = {
		ui.btnReloadResources,
		ui.btnUndo,
		ui.btnRedo,

		ui.btnDatabaseSave,
		ui.btnDatabaseLoad,
		ui.btnDatabasePropertiesAdd,

		ui.btnScenesMoveFront,
		ui.btnScenesMoveBack,
		ui.btnSceneDelete,

		ui.btnSceneAdd,
		ui.btnSceneNodeMoveFront,
		ui.btnSceneNodeMoveBack,

		ui.btnSceneClear,
		ui.btnSceneNodeDelete,
		ui.btnLabelAdd,

		ui.btnSpriteMakeBackground,
		ui.btnSpriteAdd,
		ui.btnCustomObjectAdd
	};
	for(int i = 0; i < affectedpushbuttonscount; i++)
	{
		affectedpushbuttons[i]->setEnabled(enabled);
	}	
}

bool MainPanel::isEditingEnabled() const
{
	return ui.btnLabelAdd->isEnabled() 
		&& ui.btnSpriteAdd->isEnabled()
		&& ui.btnCustomObjectAdd->isEnabled();
}

void MainPanel::setEditor(core::Editor* editor)
{  
	m_editor = editor; 

	sad::hfsm::Machine* m = editor->machine();
	sad::String la = "adding/label";
	sad::String ssa = "adding/sprite";
	sad::String sda = "adding/sprite_diagonal";
	sad::String sdap = "adding/sprite_diagonal/point_placed";
    sad::String coa = "adding/customobject";
    sad::String coad = "adding/customobject_diagonal";
	sad::String coadp = "adding/customobject_diagonal/point_placed";

	sad::String i = "idle";

    sad::String s = "selected";
	sad::String sm = "selected/moving";
	sad::String sr = "selected/resizing";

	sad::String wi = "ways/idle";
	sad::String ws = "ways/selected";
	sad::String wsm = "ways/selected/moving";

	// A bindings for idle state
	sad::Renderer::ref()->controls()->add(
		*sad::input::ET_MousePress & sad::MouseLeft & (m * i),
		m_editor->selection(),
		&core::Selection::trySelect
	);

	// A bindings for moving object
	sad::Renderer::ref()->controls()->add(
		*sad::input::ET_MouseMove & (m * sm),
		m_scene_node_actions,
		&gui::SceneNodeActions::moveObject
	);
	sad::Renderer::ref()->controls()->add(
		*sad::input::ET_MouseRelease & (m * sm),
		m_scene_node_actions,
		&gui::SceneNodeActions::commitObjectMoving
	);

	// A bindings for resizing object
	sad::Renderer::ref()->controls()->add(
		*sad::input::ET_MouseMove & (m * sr),
		m_scene_node_actions,
		&gui::SceneNodeActions::resizeObject
	);
	sad::Renderer::ref()->controls()->add(
		*sad::input::ET_MouseRelease & (m * sr),
		m_scene_node_actions,
		&gui::SceneNodeActions::commitObjectResizing
	);

	// A bindings for selected node actions
    sad::Renderer::ref()->controls()->add(
        *sad::input::ET_MouseWheel & (m * s),
        m_scene_node_actions,
        &gui::SceneNodeActions::navigateOrRotate
    );
	sad::Renderer::ref()->controls()->add(
		*sad::input::ET_KeyPress & sad::Esc & (m * s),
        m_scene_node_actions,
        &gui::SceneNodeActions::cancelSelection
    );
	sad::Renderer::ref()->controls()->add(
		*sad::input::ET_MousePress & sad::MouseLeft & (m * s),
		m_editor->selection(),
		&core::Selection::trySelect
	);

	// A bindings for adding label
	sad::Renderer::ref()->controls()->add(
		*sad::input::ET_KeyPress & sad::Esc & (m * la), 
		m_label_actions, 
		&gui::LabelActions::cancelAddLabel
	);
	sad::Renderer::ref()->controls()->add(
		*sad::input::ET_MouseMove & (m * la),
		m_label_actions,
		&gui::LabelActions::moveLabel
	);
	sad::Renderer::ref()->controls()->add(
		*sad::input::ET_MousePress & sad::MouseLeft & (m * la),
		m_label_actions,
		&gui::LabelActions::commitLabelAdd
	);
	sad::Renderer::ref()->controls()->add(
		*sad::input::ET_MouseWheel & (m * la),
        m_scene_node_actions,
        &gui::SceneNodeActions::rotate
	);


	// A binding for adding sprite actions (just placing)
	sad::Renderer::ref()->controls()->add(
		*sad::input::ET_KeyPress & sad::Esc & (m * ssa), 
		m_sprite2d_actions, 
		&gui::Sprite2DActions::cancelAddSprite
	);
	sad::Renderer::ref()->controls()->add(
		*sad::input::ET_MouseMove & (m * ssa),
		m_sprite2d_actions,
		&gui::Sprite2DActions::moveCenterOfSprite
	);
	sad::Renderer::ref()->controls()->add(
		*sad::input::ET_MousePress & sad::MouseLeft & (m * ssa),
		m_sprite2d_actions,
		&gui::Sprite2DActions::commitAdd
	);
	sad::Renderer::ref()->controls()->add(
		*sad::input::ET_MouseWheel & (m * ssa),
        m_scene_node_actions,
        &gui::SceneNodeActions::rotate
	);

	// A binding for adding sprite (after first click)
	sad::Renderer::ref()->controls()->add(
		*sad::input::ET_KeyPress & sad::Esc & (m * sdap), 
		m_sprite2d_actions, 
		&gui::Sprite2DActions::cancelAddSprite
	);
	sad::Renderer::ref()->controls()->add(
		*sad::input::ET_MousePress & sad::MouseLeft & (m * sdap),
		m_sprite2d_actions, 
		&gui::Sprite2DActions::commitAdd
	);
	sad::Renderer::ref()->controls()->add(
		*sad::input::ET_MouseMove & (m * sdap),
		m_sprite2d_actions,
		&gui::Sprite2DActions::moveLowerPointOfSprite
	);

	// A binding for adding sprite (when first click determines upper-left corner)
	sad::Renderer::ref()->controls()->add(
		*sad::input::ET_KeyPress & sad::Esc & (m * sda), 
		m_sprite2d_actions, 
		&gui::Sprite2DActions::cancelAddSprite
	);
	sad::Renderer::ref()->controls()->add(
		*sad::input::ET_MousePress & sad::MouseLeft & (m * sda),
		m_sprite2d_actions, 
		&gui::Sprite2DActions::placeFirstPointForSprite
	);

	// A binding for adding custom object actions (just placing)
	sad::Renderer::ref()->controls()->add(
		*sad::input::ET_KeyPress & sad::Esc & (m * coa), 
		m_custom_object_actions, 
		&gui::CustomObjectActions::cancelAdd
	);
	sad::Renderer::ref()->controls()->add(
		*sad::input::ET_MouseMove & (m * coa),
		m_custom_object_actions,
		&gui::CustomObjectActions::moveCenterOfObject
	);
	sad::Renderer::ref()->controls()->add(
		*sad::input::ET_MousePress & sad::MouseLeft & (m * coa),
		m_custom_object_actions,
		&gui::CustomObjectActions::commitAdd
	);
	sad::Renderer::ref()->controls()->add(
		*sad::input::ET_MouseWheel & (m * coa),
        m_scene_node_actions,
        &gui::SceneNodeActions::rotate
	);

	// A binding for adding sprite (after first click)
	sad::Renderer::ref()->controls()->add(
		*sad::input::ET_KeyPress & sad::Esc & (m * coadp), 
		m_custom_object_actions, 
		&gui::CustomObjectActions::cancelAdd
	);
	sad::Renderer::ref()->controls()->add(
		*sad::input::ET_MousePress & sad::MouseLeft & (m * coadp),
		m_custom_object_actions,
		&gui::CustomObjectActions::commitAdd
	);
	sad::Renderer::ref()->controls()->add(
		*sad::input::ET_MouseMove & (m * coadp),
		m_custom_object_actions,
		&gui::CustomObjectActions::moveLowerPoint
	);

	// A binding for adding sprite (when first click determines upper-left corner)
	sad::Renderer::ref()->controls()->add(
		*sad::input::ET_KeyPress & sad::Esc & (m * coad), 
		m_custom_object_actions, 
		&gui::CustomObjectActions::cancelAdd
	);
	sad::Renderer::ref()->controls()->add(
		*sad::input::ET_MousePress & sad::MouseLeft & (m * coad),
		m_custom_object_actions, 
		&gui::CustomObjectActions::placeFirstPoint
	);

    // A binding for ways/selected/moving
    sad::Renderer::ref()->controls()->add(
        *sad::input::ET_MouseMove & (m * wsm),
        m_way_actions,
        &gui::WayActions::moveWayPoint
    );
    sad::Renderer::ref()->controls()->add(
        *sad::input::ET_MouseRelease & (m * wsm),
        m_way_actions,
        &gui::WayActions::commitWayPointMoving
    );

    // A binding for ways/selected
    sad::Renderer::ref()->controls()->add(
        *sad::input::ET_MousePress & sad::MouseLeft & (m * ws),
        m_editor->selection(),
        &core::Selection::trySelect
    );

    // A binding for ways/idle
    sad::Renderer::ref()->controls()->add(
        *sad::input::ET_MousePress & sad::MouseLeft & (m * wi),
        m_editor->selection(),
        &core::Selection::trySelect
    );


	connect(ui.tabTypes, SIGNAL(currentChanged(int)), this, SLOT(tabTypeChanged(int)));

	connect(ui.btnDatabaseSave, SIGNAL(clicked()), this, SLOT(save()));
	connect(ui.btnDatabaseSaveAs, SIGNAL(clicked()), this, SLOT(saveAs()));
	connect(ui.btnDatabaseLoad, SIGNAL(clicked()), this, SLOT(load()));
	connect(ui.btnLoadResources, SIGNAL(clicked()), this, SLOT(loadResources()));
	connect(ui.btnReloadResources, SIGNAL(clicked()), this, SLOT(reloadResources()));

	
	connect(ui.btnDatabasePropertiesAdd, SIGNAL(clicked()), this, SLOT(addDatabaseProperty()));
	
	connect(ui.btnSceneAdd, SIGNAL(clicked()), this, SLOT(addScene()));
	connect(ui.btnSceneDelete, SIGNAL(clicked()), this, SLOT(removeScene()));
	connect(ui.lstScenes, SIGNAL(currentRowChanged(int)), this, SLOT(currentSceneChanged(int)));
	connect(ui.txtSceneName, SIGNAL(textEdited(const QString&)), this, SLOT(sceneNameChanged(const QString&)));
	connect(ui.btnScenesMoveBack, SIGNAL(clicked()), this, SLOT(sceneMoveBack()));
	connect(ui.btnScenesMoveFront, SIGNAL(clicked()), this, SLOT(sceneMoveFront()));
	connect(ui.btnSceneClear, SIGNAL(clicked()), this, SLOT(clearScene()));
	
	connect(ui.txtObjectName, SIGNAL(textEdited(const QString&)), m_scene_node_actions, SLOT(nameEdited(const QString&)));
	connect(ui.cbSceneNodeVisible, SIGNAL(clicked(bool)), m_scene_node_actions, SLOT(visibilityChanged(bool)));
    connect(ui.clpSceneNodeColor, SIGNAL(selectedColorChanged(QColor)), m_scene_node_actions, SLOT(colorChanged(QColor)));
    connect(ui.rwSceneNodeRect, SIGNAL(valueChanged(QRectF)), m_scene_node_actions, SLOT(areaChanged(QRectF)));
    connect(ui.awSceneNodeAngle, SIGNAL(valueChanged(double)), m_scene_node_actions, SLOT(angleChanged(double)));
	connect(ui.lstSceneObjects, SIGNAL(currentRowChanged(int)), this, SLOT(currentSceneNodeChanged(int)));
	connect(ui.btnSceneNodeDelete, SIGNAL(clicked()), m_scene_node_actions, SLOT(removeSceneNode()));
	connect(ui.btnSceneNodeMoveBack, SIGNAL(clicked()), this, SLOT(sceneNodeMoveBack()));
	connect(ui.btnSceneNodeMoveFront, SIGNAL(clicked()), this, SLOT(sceneNodeMoveFront()));

	connect(ui.btnLabelAdd, SIGNAL(clicked()), m_label_actions, SLOT(addLabel()));
	connect(ui.rtwLabelFont, SIGNAL(selectionChanged(sad::String)), m_label_actions, SLOT(labelFontChanged(sad::String)));
    connect(ui.fswLabelFontSize, SIGNAL(valueChanged(unsigned int)), m_label_actions, SLOT(labelSizeChanged(unsigned int)));
    connect(ui.txtLabelText, SIGNAL(textChanged()), m_label_actions, SLOT(labelTextChanged()));
	connect(ui.dsbLineSpacingRatio, SIGNAL(valueChanged(double)), m_label_actions, SLOT(labelLineSpacingChanged(double)));

	connect(ui.btnSpriteAdd, SIGNAL(clicked()), m_sprite2d_actions, SLOT(add()));
	connect(ui.rtwSpriteSprite, SIGNAL(selectionChanged(sad::String)), m_sprite2d_actions, SLOT(spriteOptionsChanged(sad::String)));
	connect(ui.btnSpriteMakeBackground, SIGNAL(clicked()), m_sprite2d_actions, SLOT(makeBackground()));
	connect(ui.cbFlipX, SIGNAL(clicked(bool)), m_sprite2d_actions, SLOT(flipXChanged(bool)));
	connect(ui.cbFlipY, SIGNAL(clicked(bool)), m_sprite2d_actions, SLOT(flipYChanged(bool)));
    
	connect(ui.btnCustomObjectAdd, SIGNAL(clicked()), m_custom_object_actions, SLOT(add()));
	connect(ui.rtwCustomObjectSchemas, SIGNAL(selectionChanged(sad::String)), m_custom_object_actions, SLOT(schemaChanged(sad::String)));

    connect(ui.lstWays, SIGNAL(currentRowChanged(int)), m_way_actions, SLOT(wayChanged(int)));
    connect(ui.btnWayAdd, SIGNAL(clicked()), m_way_actions, SLOT(addWay()));
    connect(ui.btnWayRemove, SIGNAL(clicked()), m_way_actions, SLOT(removeWay()));
    connect(ui.txtWayName, SIGNAL(textEdited(const QString&)), m_way_actions, SLOT(nameEdited(const QString&)));
    connect(ui.cbWayClosed, SIGNAL(clicked(bool)), m_way_actions, SLOT(closednessChanged(bool)));
    connect(ui.dsbWayTotalTime, SIGNAL(valueChanged(double)), m_way_actions, SLOT(totalTimeChanged(double)));
    connect(ui.lstWayPoints, SIGNAL(currentRowChanged(int)), m_way_actions, SLOT(viewPoint(int)));
    connect(ui.btnWayPointAdd, SIGNAL(clicked()), m_way_actions, SLOT(addWayPoint()));
    connect(ui.btnWayPointRemove, SIGNAL(clicked()), m_way_actions, SLOT(removeWayPoint()));
	connect(ui.dsbWayPointX, SIGNAL(valueChanged(double)), m_way_actions, SLOT(wayPointXChanged(double)));
	connect(ui.dsbWayPointY, SIGNAL(valueChanged(double)), m_way_actions, SLOT(wayPointYChanged(double)));
	connect(ui.btnWayPointMoveBack, SIGNAL(clicked()), m_way_actions, SLOT(wayPointMoveBack()));
	connect(ui.btnWayPointMoveFront, SIGNAL(clicked()), m_way_actions, SLOT(wayPointMoveFront()));
}

core::Editor* MainPanel::editor() const
{
    return m_editor;
}

gui::SceneNodeActions* MainPanel::sceneNodeActions() const
{
    return m_scene_node_actions;
}

gui::LabelActions* MainPanel::labelActions() const
{
    return m_label_actions;
}

gui::Sprite2DActions* MainPanel::sprite2DActions() const
{
	return m_sprite2d_actions;
}

gui::CustomObjectActions* MainPanel::customObjectActions() const
{
	return m_custom_object_actions;
}

gui::WayActions* MainPanel::wayActions() const
{
	return m_way_actions;
}

Ui::MainPanelClass* MainPanel::UI()
{
    return &ui;
}

void MainPanel::viewDatabase()
{
	this->fixDatabase();
	sad::db::Database* db = sad::Renderer::ref()->database("");
	ui.clpSceneNodeColor->setPalette(db->getProperty<QList<QList<QColor> > >("palette").value());

	// Remove old delegates
	for(sad::PtrHash<sad::String, gui::table::Delegate>::iterator it = m_property_delegates.begin();
		it != m_property_delegates.end();
		++it)
	{
		it.value()->delRef();
	}

	for(sad::db::Database::Properties::const_iterator it = db->begin();
		it != db->end();
		++it)
	{
		// Skip palette
		if (it.key() != "palette" && it.value()->pointerStarsCount() == 0)
		{
			gui::table::Delegate* d = m_dbdelegate_factory.create(it.value()->baseType().c_str());
			if (d)
			{
				d->makeLinkedTo(ui.twDatabaseProperties, m_editor);
				d->setPropertyName(it.key().c_str());
				d->linkToDatabase();
				d->add();				
			}
		}
	}

	sad::db::populateScenesFromDatabase(sad::Renderer::ref(), sad::Renderer::ref()->database(""));

	const sad::Vector<sad::Scene*>& scenes = sad::Renderer::ref()->scenes(); 
	for(unsigned int i = 0; i < scenes.size(); i++)
	{
		addSceneToSceneList(scenes[i]);
	}

    sad::Vector<sad::db::Object*> wayslist;
    db->table("ways")->objects(wayslist);
    for(unsigned int i = 0; i < wayslist.size(); i++)
    {
        sad::p2d::app::Way* w = static_cast<sad::p2d::app::Way*>(wayslist[i]);
        addLastWayToEnd(w);
    }
}

QList<QList<QColor> >  MainPanel::colorPalette() const
{
	return ui.clpSceneNodeColor->palette();
}

void MainPanel::setColorPalette(const QList<QList<QColor> >& palette)
{
	ui.clpSceneNodeColor->setPalette(palette);
	sad::db::Database* db = sad::Renderer::ref()->database("");
	db->setProperty("palette", palette);
}

bool  MainPanel::takeDelegateByPropertyName(const QString & name)
{
	bool owns = false;
	if (m_property_delegates.contains(name.toStdString()))
	{
		m_property_delegates.remove(name.toStdString());
	}
	return owns;
}

void MainPanel::addSceneToSceneList(sad::Scene* s)
{
	QString name = this->viewableObjectName(s);
	QListWidgetItem* i =  new QListWidgetItem();
	i->setText(name);
	
	QVariant v;
	v.setValue(s);
	i->setData(Qt::UserRole, v);
	ui.lstScenes->addItem(i);
}

void MainPanel::removeLastSceneFromSceneList()
{
	if (ui.lstScenes->count())
	{
		QListWidgetItem* i = ui.lstScenes->takeItem(ui.lstScenes->count() - 1);
		delete i;
	}
}

void MainPanel::insertSceneToSceneList(sad::Scene* s, int position)
{
	QString name = this->viewableObjectName(s);
	QListWidgetItem* i =  new QListWidgetItem();
	i->setText(name);
	
	QVariant v;
	v.setValue(s);
	i->setData(Qt::UserRole, v);
	ui.lstScenes->insertItem(position, i);
}

void MainPanel::removeSceneFromSceneList(int position)
{
	QListWidgetItem* i =  ui.lstScenes->takeItem(position);
	delete i;
}

sad::Scene* MainPanel::currentScene()
{
	QListWidgetItem* item = ui.lstScenes->currentItem();
	sad::Scene* scene = NULL;
	if (item)
	{
		scene = item->data(Qt::UserRole).value<sad::Scene*>();
	}
	return scene;
}

void MainPanel::updateSceneName(sad::Scene* s)
{
	int row = this->findSceneInList(s);
	if (row != -1)
	{
		ui.lstScenes->item(row)->setText(this->viewableObjectName(s));
	}
	if (s == currentScene())
	{
		bool b = ui.txtSceneName->blockSignals(true);
		ui.txtSceneName->setText(s->objectName().c_str());
		ui.txtSceneName->blockSignals(b);
	}
}

int MainPanel::findSceneInList(sad::Scene* s)
{
	int row = -1;
	for(int i = 0; i < ui.lstScenes->count(); i++)
	{	
		if (ui.lstScenes->item(i)->data(Qt::UserRole).value<sad::Scene*>() == s)
		{
			row = i;
		}
	}
	return row;
}

void MainPanel::setScenesInList(sad::Scene* s1, sad::Scene* s2, int pos1, int pos2)
{
	sad::Scene* s = this->currentScene();
	ui.lstScenes->item(pos1)->setText(this->viewableObjectName(s1));
	QVariant v1;
	v1.setValue(s1);
	ui.lstScenes->item(pos1)->setData(Qt::UserRole, v1);

	ui.lstScenes->item(pos2)->setText(this->viewableObjectName(s2));
	QVariant v2;
	v2.setValue(s2);
	ui.lstScenes->item(pos2)->setData(Qt::UserRole, v2);

	if (s == s1 || s == s2)
	{
		this->currentSceneChanged(ui.lstScenes->currentRow());
	}
}

void MainPanel::updateMousePosition(const sad::input::MouseMoveEvent & e)
{
	m_mousemove_point = e.pos2D();
	QTimer::singleShot(0, this, SLOT(updateMousePositionNow()));
}

void MainPanel::updateResourceViews()
{
    ui.rtwLabelFont->setTree("");
    ui.rtwLabelFont->setFilter("sad::freetype::Font|sad::TextureMappedFont");
    ui.rtwLabelFont->updateTree();

    ui.rtwSpriteSprite->setTree("");
    ui.rtwSpriteSprite->setFilter("sad::Sprite2D::Options");
    ui.rtwSpriteSprite->updateTree();

    ui.rtwCustomObjectSchemas->setTree("");
    ui.rtwCustomObjectSchemas->setFilter("sad::db::custom::Schema");
    ui.rtwCustomObjectSchemas->updateTree();
}

void MainPanel::highlightState(const sad::String & text)
{
	m_highlight_state = text.c_str();
	QTimer::singleShot(0, this, SLOT(highlightStateNow()));
}

void MainPanel::highlightIdleState()
{
    this->highlightState("Idle");
}

void MainPanel::highlightSelectedState()
{
    this->highlightState("Editing item...");
}

void MainPanel::highlightLabelAddingState()
{
	this->highlightState("Click, where you want label to be placed");
}

void MainPanel::addSceneNodeToSceneNodeList(sad::SceneNode* s)
{
	QString name = this->viewableObjectName(s);
	QListWidgetItem* i =  new QListWidgetItem();
	i->setText(name);
	
	QVariant v;
	v.setValue(s);
	i->setData(Qt::UserRole, v);
	ui.lstSceneObjects->addItem(i);
}

void MainPanel::removeLastSceneNodeFromSceneNodeList()
{
	if (ui.lstSceneObjects->count())
	{
		QListWidgetItem* i = ui.lstSceneObjects->takeItem(ui.lstSceneObjects->count() - 1);
		delete i;
	}
}

void MainPanel::insertSceneNodeToSceneNodeList(sad::SceneNode* s, int position)
{
	QString name = this->viewableObjectName(s);
	QListWidgetItem* i =  new QListWidgetItem();
	i->setText(name);
	
	QVariant v;
	v.setValue(s);
	i->setData(Qt::UserRole, v);
	ui.lstSceneObjects->insertItem(position, i);
}

void MainPanel::removeSceneNodeFromSceneNodeList(int position)
{
	QListWidgetItem* i =  ui.lstSceneObjects->takeItem(position);
	delete i;
}

void MainPanel::removeSceneNodeFromSceneNodeListByNode(sad::SceneNode* s)
{
	int position = this->findSceneNodeInList(s);
	if (position >= 0)
	{
		removeSceneNodeFromSceneNodeList(position);
	}
}

void MainPanel::setSceneNodesInList(sad::SceneNode* n1, sad::SceneNode* n2, int pos1, int pos2)
{
	sad::SceneNode* s = this->editor()->shared()->selectedObject();
	ui.lstSceneObjects->item(pos1)->setText(this->viewableObjectName(n1));
	QVariant v1;
	v1.setValue(n1);
	ui.lstSceneObjects->item(pos1)->setData(Qt::UserRole, v1);

	ui.lstSceneObjects->item(pos2)->setText(this->viewableObjectName(n2));
	QVariant v2;
	v2.setValue(n2);
	ui.lstSceneObjects->item(pos2)->setData(Qt::UserRole, v2);

	if (s == n1)
	{
		void (QListWidget::*row)(int) = &QListWidget::setCurrentRow;
		invoke_blocked(ui.lstSceneObjects, row, pos1);
		this->currentSceneNodeChanged(ui.lstSceneObjects->currentRow());
	}
	if (s == n2)
	{
		void (QListWidget::*row)(int) = &QListWidget::setCurrentRow;
		invoke_blocked(ui.lstSceneObjects, row, pos2);
		this->currentSceneNodeChanged(ui.lstSceneObjects->currentRow());
	}
}

int MainPanel::findSceneNodeInList(sad::SceneNode* s)
{
	int row = -1;
	for(int i = 0; i < ui.lstSceneObjects->count(); i++)
	{	
		if (ui.lstSceneObjects->item(i)->data(Qt::UserRole).value<sad::SceneNode*>() == s)
		{
			row = i;
		}
	}
	return row;
}

void MainPanel::updateSceneNodeName(sad::SceneNode* s)
{
	int row = this->findSceneNodeInList(s);
	if (row != -1)
	{
		ui.lstSceneObjects->item(row)->setText(this->viewableObjectName(s));
	}
	if (s == m_editor->shared()->selectedObject() || s == m_editor->shared()->activeObject())
	{
		bool b = ui.txtSceneName->blockSignals(true);
		ui.txtSceneName->setText(s->objectName().c_str());
		ui.txtSceneName->blockSignals(b);
	}
}

void MainPanel::addLastWayToEnd(sad::p2d::app::Way* way)
{
    ui.lstWays->addItem(this->viewableObjectName(way));
    QVariant v;
    v.setValue(way);
    ui.lstWays->item(ui.lstWays->count()-1)->setData(Qt::UserRole, v);
}

void MainPanel::removeLastWayFromWayList()
{
    if (ui.lstWays->count() > 0)
    {
        QVariant v = ui.lstWays->item(ui.lstWays->count() - 1)->data(Qt::UserRole);
        sad::p2d::app::Way* w  = v.value<sad::p2d::app::Way*>();
        if (w == m_editor->shared()->selectedWay())
        {
            m_editor->shared()->setSelectedWay(NULL);
            m_editor->machine()->enterState("ways/idle");
        }
        delete ui.lstWays->takeItem(ui.lstWays->count() - 1);
    }
}

void MainPanel::insertWayToWayList(sad::p2d::app::Way* s, int position)
{
    QListWidgetItem* i = new QListWidgetItem(this->viewableObjectName(s));
    QVariant v;
    v.setValue(s);
    i->setData(Qt::UserRole, v);
    ui.lstWays->insertItem(position, i);
}

void MainPanel::removeWayFromWayList(int position)
{
    QVariant v = ui.lstWays->item(position)->data(Qt::UserRole);
    sad::p2d::app::Way* w  = v.value<sad::p2d::app::Way*>();
    if (w == m_editor->shared()->selectedWay())
    {
        m_editor->shared()->setSelectedWay(NULL);
        m_editor->machine()->enterState("ways/idle");
    }
    delete ui.lstWays->takeItem(position);
}

void MainPanel::removeWayFromWayList(sad::p2d::app::Way* s)
{
    int pos = this->findWayInList(s);
    if (s == m_editor->shared()->selectedWay())
    {
        m_editor->shared()->setSelectedWay(NULL);
        m_editor->machine()->enterState("ways/idle");
    }
    if (pos >= 0)
    {
        delete ui.lstWays->takeItem(pos);
    }
}

int MainPanel::findWayInList(sad::p2d::app::Way* s)
{
    for(int i = 0; i < ui.lstWays->count(); i++)
    {
        QVariant v = ui.lstWays->item(i)->data(Qt::UserRole);
        sad::p2d::app::Way* w  = v.value<sad::p2d::app::Way*>();
        if (w == s)
        {
            return i;
        }
    }
    return -1;
}

void MainPanel::updateWayName(sad::p2d::app::Way* s)
{
    int row = this->findWayInList(s);
    if (row != -1)
    {
        ui.lstWays->item(row)->setText(this->viewableObjectName(s));
    }
}

void MainPanel::removeRowInWayPointList(int row)
{
    if (row >=0  && row < ui.lstWayPoints->count()) {
        delete ui.lstWayPoints->takeItem(row);
    }
}

QString MainPanel::nameForPoint(const sad::Point2D& p) const
{
    return QString("(%1,%2)")
           .arg(static_cast<int>(p.x()))
           .arg(static_cast<int>(p.y()));
}

QCheckBox* MainPanel::visibilityCheckbox() const
{
	return ui.cbSceneNodeVisible;	
}

QCheckBox* MainPanel::flipXCheckbox() const
{
	return ui.cbFlipX;	
}

QCheckBox* MainPanel::flipYCheckbox() const
{
	return ui.cbFlipY;	
}

void MainPanel::clearCustomObjectPropertiesTable()
{
    for(size_t i = 0; i < ui.twCustomObjectProperties->rowCount(); i++)
    {
        QVariant  v = ui.twCustomObjectProperties->item(i, 0)->data(Qt::UserRole);
        gui::table::Delegate* d = v.value<gui::table::Delegate*>();
        if (d)
        {
            d->disconnectSlots();
            delete d;
        }
    }
    ui.twCustomObjectProperties->clear();
	ui.twCustomObjectProperties->setRowCount(0);
}

 gui::table::Delegate* MainPanel::delegateForCustomObjectProperty(const QString& name)
 {
     for(size_t i = 0; i < ui.twCustomObjectProperties->rowCount(); i++)
     {
         if (ui.twCustomObjectProperties->item(i, 0)->text() == name) {
             QVariant  v = ui.twCustomObjectProperties->item(i, 0)->data(Qt::UserRole);
             gui::table::Delegate* d = v.value<gui::table::Delegate*>();
             return d;
         }
     }
     return NULL;
 }

void MainPanel::updateCustomObjectPropertyValue(
     sad::SceneNode* node,
     const sad::String& name,
     const sad::db::Variant& value
)
{
    if (m_editor->isNodeSelected(node))
    {
        m_custom_object_property_name = name;
        m_custom_object_property_value = value;
        QTimer::singleShot(0, this, SLOT(updateCustomObjectPropertyValueNow()));
    }
}

void MainPanel::fillCustomObjectProperties(
	sad::SceneNode* node	
)
{
	this->clearCustomObjectPropertiesTable();
	if (node)
	{
		if (node->metaData()->canBeCastedTo("sad::db::custom::Object"))
		{
			sad::db::custom::Object* o = static_cast<sad::db::custom::Object*>(node);
			const sad::Hash<sad::String, sad::db::Property*>& db = o->schemaProperties();
			for(sad::Hash<sad::String, sad::db::Property*>::const_iterator it = db.const_begin();
				it != db.const_end();
				++it)
			{
				gui::table::Delegate* d = m_dbdelegate_factory.create(it.value()->baseType().c_str());
				if (d)
				{
					d->makeLinkedTo(ui.twCustomObjectProperties, m_editor);
					d->setPropertyName(it.key().c_str());
					d->linkToCustomObject(o);
					d->add();				
				}
			}
		}
	}
}

void MainPanel::selectLastSceneNode()
{
	if (ui.lstSceneObjects->count() != 0)
	{
		bool b = ui.lstSceneObjects->blockSignals(true);
		ui.lstSceneObjects->setCurrentRow(ui.lstSceneObjects->count() - 1);
		ui.lstSceneObjects->blockSignals(b);
	}
}

//====================  PUBLIC SLOTS METHODS HERE ====================

void MainPanel::updateUIForSelectedItem()
{
	QTimer::singleShot(0, this, SLOT(updateUIForSelectedItemNow()));
}

void MainPanel::updateUIForSelectedItemNow()
{
	sad::SceneNode* node = m_editor->shared()->selectedObject();
	if (node)
	{
        // Scene tab
        int row = this->findSceneNodeInList(node);
        if (row != ui.lstSceneObjects->currentRow()) {
            void (QListWidget::*setRow)(int) = &QListWidget::setCurrentRow;
            invoke_blocked(ui.lstSceneObjects, setRow, row);
        }
        invoke_blocked(ui.txtObjectName, &QLineEdit::setText, node->objectName().c_str());

        // SceneNode tab
        m_scene_node_actions->updateRegionForNode();
        sad::Maybe<bool> maybevisible = node->getProperty<bool>("visible");
        if (maybevisible.exists())
        {
            invoke_blocked(ui.cbSceneNodeVisible, &QCheckBox::setCheckState, (maybevisible.value()) ? Qt::Checked : Qt::Unchecked);
        }
        gui::UpdateElement<double>::with(node, "angle", ui.awSceneNodeAngle, &gui::anglewidget::AngleWidget::setValue);
        gui::UpdateElement<QColor>::with(node, "color", ui.clpSceneNodeColor, &gui::colorpicker::ColorPicker::setSelectedColor);

        // Label tab
        gui::UpdateElement<sad::String>::with(node, "font", ui.rtwLabelFont, &gui::resourcetreewidget::ResourceTreeWidget::setSelectedResourceName);
        gui::UpdateElement<unsigned int>::with(node, "fontsize", ui.fswLabelFontSize, &gui::fontsizewidget::FontSizeWidget::setValue);
        gui::UpdateElement<float>::with(node, "linespacing", ui.dsbLineSpacingRatio, &QDoubleSpinBox::setValue);
        gui::UpdateElement<QString>::with(node, "text", ui.txtLabelText, &QPlainTextEdit::setPlainText);

        // Sprite2D tab
        gui::UpdateElement<sad::String>::with(node, "options", ui.rtwSpriteSprite, &gui::resourcetreewidget::ResourceTreeWidget::setSelectedResourceName);
        sad::Maybe<bool> maybeflipx = node->getProperty<bool>("flipx");
        if (maybeflipx.exists())
        {
            invoke_blocked(ui.cbFlipX, &QCheckBox::setCheckState, (maybeflipx.value()) ? Qt::Checked : Qt::Unchecked);
        }
        sad::Maybe<bool> maybeflipy = node->getProperty<bool>("flipy");
        if (maybeflipy.exists())
        {
            invoke_blocked(ui.cbFlipY, &QCheckBox::setCheckState, (maybeflipy.value()) ? Qt::Checked : Qt::Unchecked);
        }


        // Custom object tab
        gui::UpdateElement<sad::String>::with(node, "schema", ui.rtwCustomObjectSchemas, &gui::resourcetreewidget::ResourceTreeWidget::setSelectedResourceName);
		if (node->metaData()->canBeCastedTo("sad::db::custom::Object"))
		{
			this->fillCustomObjectProperties(node);
        }
        else
        {
            this->clearCustomObjectPropertiesTable();
        }
	}
}

void MainPanel::updateCustomObjectPropertyValueNow()
{
    gui::table::Delegate* d = this->delegateForCustomObjectProperty(m_custom_object_property_name.data());
    if (d)
    {
        d->set(m_custom_object_property_value);
    }
}

void MainPanel::updateUIForSelectedWay()
{
    QTimer::singleShot(0, this, SLOT(updateUIForSelectedWayNow()));
}

void MainPanel::updateUIForSelectedWayNow()
{
    if (m_editor->shared()->selectedWay())
    {
        ui.lstWayPoints->clear();
        sad::p2d::app::Way* p = m_editor->shared()->selectedWay();
        for(size_t i = 0; i < p->wayPoints().size(); ++i)
        {
            ui.lstWayPoints->addItem(this->nameForPoint(p->wayPoints()[i]));
        }
        invoke_blocked(ui.txtWayName, &QLineEdit::setText, p->objectName().c_str());
        invoke_blocked(ui.dsbWayTotalTime, &QDoubleSpinBox::setValue, p->totalTime());
        invoke_blocked(ui.cbWayClosed, &QCheckBox::setCheckState,  (p->closed()) ? Qt::Checked : Qt::Unchecked);
    }
}

//====================  PROTECTED METHODS HERE ====================

void MainPanel::fillDatabasePropertyTypesCombo()
{
    const unsigned int typescount = 20;
    const QString types[typescount] = {
        "unsigned char",
        "signed char",
        "char",

        "short",
        "unsigned short",

        "int",
        "unsigned int",

        "long",
        "unsigned long",

        "long long",
        "unsigned long long",

        "float",
        "double",

        "sad::String",
        "sad::Color",
        "sad::AColor",

        "sad::Point2D",
        "sad::Point2I",

        "sad::Size2D",
        "sad::Size2I"
    };
    for(unsigned int i = 0; i < typescount; i++)
    {
        ui.cmbDatabasePropertyType->addItem(types[i]);
    }
}

void MainPanel::closeEvent(QCloseEvent* ev)
{
	this->QMainWindow::closeEvent(ev);
}

void MainPanel::fixDatabase()
{
	sad::db::Database* db = sad::Renderer::ref()->database("");
    // Contains sad::Scene
    if (db->table("scenes") == NULL)
	{
		db->addTable("scenes", new sad::db::Table());
	}
    // Contains sad::SceneNode
	if (db->table("scenenodes") == NULL)
	{
		db->addTable("scenenodes", new sad::db::Table());
	}
    // Contains sad::p2d::app::Way
    if (db->table("ways") == NULL)
    {
        db->addTable("ways", new sad::db::Table());
    }
    // Contains sad::dialogue::Dialogue
    if (db->table("dialogues") == NULL)
    {
        db->addTable("dialogues", new sad::db::Table());
    }

	bool needtosetpalette = false;
	if (db->propertyByName("palette") != NULL)
	{
		if (db->propertyByName("palette")->baseType() != "sad::Vector<sad::Vector<sad::AColor> >"
			|| db->propertyByName("palette")->pointerStarsCount() != 0)
		{
			needtosetpalette = true;
			db->removeProperty("palette");
			db->addProperty(
				"palette", 
				new sad::db::StoredProperty<sad::Vector<sad::Vector<sad::AColor> > >()
			);
		}
	} 
	else
	{
		needtosetpalette = true;
		db->addProperty(
			"palette", 
			new sad::db::StoredProperty<sad::Vector<sad::Vector<sad::AColor> > >()
		);
	}
	// Init palette
	if (needtosetpalette)
	{
		sad::Vector<sad::Vector<sad::AColor> > default_palette;

		default_palette << sad::Vector<sad::AColor >();
        default_palette[0] << sad::AColor(255, 255, 255, 0);
        default_palette[0] << sad::AColor(255, 0, 0, 0);
        default_palette[0] << sad::AColor(0, 255, 0, 0);
		default_palette[0] << sad::AColor(0, 255, 255, 0);
		default_palette[0] << sad::AColor(0, 0, 255, 0);

		default_palette << sad::Vector<sad::AColor >();
        default_palette[1] << sad::AColor(192, 192, 192, 0);
		default_palette[1] << sad::AColor(192, 0, 0, 0);
		default_palette[1] << sad::AColor(0, 192, 0, 0);
		default_palette[1] << sad::AColor(0, 192, 192, 0);
		default_palette[1] << sad::AColor(0, 0, 192, 0);

		default_palette << sad::Vector<sad::AColor >();
        default_palette[2] << sad::AColor(164, 164, 164, 0);
		default_palette[2] << sad::AColor(164, 0, 0, 0);
		default_palette[2] << sad::AColor(0, 164, 0, 0);
		default_palette[2] << sad::AColor(0, 164, 164, 0);
		default_palette[2] << sad::AColor(0, 0, 164, 0);

		default_palette << sad::Vector<sad::AColor >();
        default_palette[3] << sad::AColor(128, 128, 128, 0);
        default_palette[3] << sad::AColor(128, 0, 0, 0);
		default_palette[3] << sad::AColor(0, 128, 0, 0);
		default_palette[3] << sad::AColor(0, 128, 128, 0);
		default_palette[3] << sad::AColor(0, 0, 128, 0);
		db->setProperty("palette", default_palette);
	}
}

QString MainPanel::viewableObjectName(sad::db::Object* o)
{
	QString result = o->objectName().c_str();
	if (result.length() == 0)
	{
		char buffer[20];
		sprintf(buffer, "%p", o);
		result = QString(buffer);
	}
	return result;
}

//====================  PROTECTED SLOTS HERE ====================

void MainPanel::addDatabaseProperty()
{
	sad::db::Database* db = sad::Renderer::ref()->database("");
	sad::String propname = ui.txtDatabasePropertyName->text().toStdString();
	if (db->propertyByName(propname) == NULL && propname.size()!= 0)
	{
		gui::table::Delegate* d  = m_dbdelegate_factory.create(ui.cmbDatabasePropertyType->currentText());
		sad::db::Property* prop = m_property_factory.create(ui.cmbDatabasePropertyType->currentText().toStdString());
		if (d && prop)
		{
			sad::Renderer::ref()->database("")->addProperty(propname, prop);
			d->setPropertyName(propname.c_str());
			d->linkToDatabase();
			d->makeLinkedTo(ui.twDatabaseProperties, m_editor);
			d->add();
			history::database::NewProperty* p = new history::database::NewProperty(d);
			m_editor->history()->add(p);
		}
		else
		{
			delete d;
			delete prop;
		}
	}
}

void MainPanel::addScene()
{
	sad::Scene* s  = new sad::Scene();

	QString name = ui.txtSceneName->text();
	if (name.length())
	{
		s->setObjectName(name.toStdString());
	}

	sad::Renderer::ref()->add(s);
	sad::Renderer::ref()->database("")->table("scenes")->add(s);

	history::Command* c = new history::scenes::Add(s);
	c->commit(m_editor);
	m_editor->history()->add(c);

	invoke_blocked<QListWidget, void (QListWidget::*)(int), int>(ui.lstScenes, &QListWidget::setCurrentRow, ui.lstScenes->count() - 1);
}

void MainPanel::currentSceneChanged(int index)
{
	if (m_editor->machine()->isInState("adding")
		|| (m_editor->machine()->isInState("selected") 
		    && m_editor->machine()->currentState() != "selected"))
	{
		if (this->currentScene())
		{
			bool b = ui.lstScenes->blockSignals(true);
			int row = this->findSceneInList(this->currentScene());
			if (row != -1)
			{
				ui.lstScenes->setCurrentRow(row);
			}
			ui.lstScenes->blockSignals(b);
			return;
		}
	}
	if (index != -1) 
	{
		QListWidgetItem* i = ui.lstScenes->item(index);
		sad::Scene* s = i->data(Qt::UserRole).value<sad::Scene*>();
		bool b = ui.txtSceneName->blockSignals(true);
		ui.txtSceneName->setText(s->objectName().c_str());
		ui.txtSceneName->blockSignals(b);

		ui.lstSceneObjects->clear();
		const sad::Vector<sad::SceneNode*>& nodes = s->objects();
		for(size_t i = 0; i < nodes.size(); i++)
		{
			if (nodes[i]->active())
			{
				QListWidgetItem* ki = new QListWidgetItem();
				ki->setText(this->viewableObjectName(nodes[i]));
				
				QVariant v;
				v.setValue(nodes[i]);
				ki->setData(Qt::UserRole, v);
				ui.lstSceneObjects->addItem(ki);
			}
		}
	}
}

void MainPanel::sceneNameChanged(const QString& name)
{
	sad::Scene* scene = currentScene();
	if (scene)
	{
		sad::String oldname = scene->objectName();
		sad::String newname = ui.txtSceneName->text().toStdString();
		
		history::Command* c = new history::scenes::ChangeName(scene, oldname, newname);
		this->m_editor->history()->add(c);
		c->commit(m_editor);
	}
}

void MainPanel::currentSceneNodeChanged(int index)
{
	if (m_editor->machine()->isInState("adding")
		|| (m_editor->machine()->isInState("selected") 
		    && m_editor->machine()->currentState() != "selected"))
	{
		if (this->editor()->shared()->selectedObject())
		{
			bool b = ui.lstSceneObjects->blockSignals(true);
			int row = this->findSceneNodeInList(this->editor()->shared()->selectedObject());
			if (row != -1)
			{
				ui.lstSceneObjects->setCurrentRow(row);
			}
			ui.lstSceneObjects->blockSignals(b);
			return;
		}
	}
	if (index != -1) 
	{
		QListWidgetItem* i = ui.lstSceneObjects->item(index);
		sad::SceneNode* s = i->data(Qt::UserRole).value<sad::SceneNode*>();

		if (m_editor->machine()->isInState("idle"))
		{
			m_editor->machine()->enterState("selected");
		}
		if (m_editor->machine()->isInState("selected"))
		{
			m_editor->shared()->setSelectedObject(s);
			this->updateUIForSelectedItem();
		}
	}
}

void MainPanel::removeScene()
{
	if (m_editor->isInEditingState())
	{
		return;
	}
	sad::Scene* scene = currentScene();
	if (scene)
	{
		if (m_editor->machine()->isInState("selected"))
		{
			m_editor->machine()->enterState("idle");
			m_editor->shared()->setSelectedObject(NULL);
		}
		int row = ui.lstScenes->currentRow();

		history::Command* c = new history::scenes::Remove(scene, row);
		this->m_editor->history()->add(c);
		c->commit(m_editor);
	}
}

void MainPanel::sceneMoveBack()
{
	sad::Scene* scene = currentScene();
	if (scene)
	{
		int row = ui.lstScenes->currentRow();
		if (row != 0)
		{
			sad::Scene* previousscene = ui.lstScenes->item(row -1)->data(Qt::UserRole).value<sad::Scene*>();

			history::Command* c = new history::scenes::LayerSwap(scene, previousscene, row, row - 1);
			this->m_editor->history()->add(c);
			c->commit(m_editor);
		}
	}
}

void MainPanel::sceneMoveFront()
{
	sad::Scene* scene = currentScene();
	if (scene)
	{
		int row = ui.lstScenes->currentRow();
		if (row < ui.lstScenes->count() - 1)
		{
			sad::Scene* nextscene = ui.lstScenes->item(row + 1)->data(Qt::UserRole).value<sad::Scene*>();

			history::Command* c = new history::scenes::LayerSwap(scene, nextscene, row, row + 1);
			this->m_editor->history()->add(c);
			c->commit(m_editor);
		}
	}
}

void MainPanel::clearScene()
{
	sad::Scene* scene = currentScene();
	if (scene)
	{
		history::Command* c = new history::scenes::Clear(scene);
		this->m_editor->history()->add(c);
		c->commit(m_editor);
	}
}

void MainPanel::updateMousePositionNow()
{
	ui.txtMousePosX->setText(QString::number(static_cast<int>(m_mousemove_point.x())));
	ui.txtMousePosY->setText(QString::number(static_cast<int>(m_mousemove_point.y())));
}

void MainPanel::highlightStateNow()
{
	ui.txtEditorState->setText(m_highlight_state);
}

void MainPanel::undo()
{
	m_editor->undo();
}

void MainPanel::redo()
{
	m_editor->redo();
}

void MainPanel::sceneNodeMoveBack()
{
	if (m_editor->machine()->isInState("selected"))
	{
		sad::SceneNode* node = m_editor->shared()->selectedObject();
		if (node)
		{
			int row = ui.lstSceneObjects->currentRow();
			int row2 = this->findSceneNodeInList(node);
			if (row > 0 && row == row2)
			{
				sad::SceneNode* previousnode = ui.lstSceneObjects->item(row - 1)->data(Qt::UserRole).value<sad::SceneNode*>();

				history::Command* c = new history::scenenodes::LayerSwap(node, previousnode, row, row - 1);
				this->m_editor->history()->add(c);
				c->commit(m_editor);

				invoke_blocked<QListWidget, void (QListWidget::*)(int), int>(ui.lstSceneObjects, &QListWidget::setCurrentRow, row - 1);
			}
		}
	}
}

void MainPanel::sceneNodeMoveFront()
{
	if (m_editor->machine()->isInState("selected"))
	{
		sad::SceneNode* node = m_editor->shared()->selectedObject();
		if (node)
		{
			int row = ui.lstSceneObjects->currentRow();
			int row2 = this->findSceneNodeInList(node);
			if (row < ui.lstSceneObjects->count() - 1 && row > -1 && row == row2)
			{
				sad::SceneNode* nextnode = ui.lstSceneObjects->item(row + 1)->data(Qt::UserRole).value<sad::SceneNode*>();

				history::Command* c = new history::scenenodes::LayerSwap(node, nextnode, row, row + 1);
				this->m_editor->history()->add(c);
				c->commit(m_editor);

				invoke_blocked<QListWidget, void (QListWidget::*)(int), int>(ui.lstSceneObjects, &QListWidget::setCurrentRow, row + 1);
			}
		}
	}
}

void MainPanel::synchronizeDatabase()
{
	ui.rtwSpriteSprite->setFilter("sad::Sprite2D::Options");
	ui.rtwLabelFont->setFilter("sad::freetype::Font|sad::TextureMappedFont");
}

void MainPanel::setAddingEnabled(bool enabled)
{
	this->ui.lstSceneObjects->setEnabled(enabled);
	this->ui.btnLabelAdd->setEnabled(enabled);
	this->ui.btnSpriteAdd->setEnabled(enabled);
}

void MainPanel::setSpriteChangingEnabled(bool enabled)
{
	this->ui.rwSceneNodeRect->setEnabled(enabled);
}

void MainPanel::setAngleChangingEnabled(bool enabled)
{
	ui.awSceneNodeAngle->setEnabled(enabled);
	if (enabled == false)
	{
		ui.awSceneNodeAngle->setValue(0);
	}
}

void MainPanel::clearDatabaseProperties()
{
	ui.twDatabaseProperties->clear();
	ui.twDatabaseProperties->setRowCount(0);
	for(sad::PtrHash<sad::String, gui::table::Delegate>::iterator it = m_property_delegates.begin();
		it != m_property_delegates.end();
		++it)
	{
		it.value()->delRef();
	}
	m_property_delegates.clear();
}

void MainPanel::save()
{
	if (m_editor->shared()->fileName().length() == 0)
	{
		this->saveAs();
	}
	else
	{
		sad::Renderer::ref()->database("")->saveToFile(m_editor->shared()->fileName().toStdString());
	}
}

void MainPanel::saveAs()
{
	QString name = QFileDialog::getSaveFileName(this, "Enter file, where we should store database", "", "*.json");
	if (name.length() != 0)
	{
		m_editor->shared()->setFileName(name);
		sad::Renderer::ref()->database("")->saveToFile(m_editor->shared()->fileName().toStdString());
	}
}

void MainPanel::load()
{
	if (m_editor->isInEditingState())
	{
		return;
	}
    QString name = QFileDialog::getOpenFileName(this, "Enter file, where database was stored", "", "*.json");
	if (name.length() != 0)
	{
		sad::db::Database* tmp = new sad::db::Database();
		tmp->setRenderer(sad::Renderer::ref());
		tmp->setDefaultTreeName("");
		if (tmp->loadFromFile(name.toStdString(), sad::Renderer::ref()))
		{
			m_editor->shared()->setFileName(name);
			sad::Renderer::ref()->lockRendering();
			m_editor->cleanDatabase();
			sad::Renderer::ref()->addDatabase("", tmp);
			this->viewDatabase();
			sad::Renderer::ref()->unlockRendering();
		}
		else
		{
			delete tmp;
			QMessageBox::critical(NULL, "Failed to load database", "Failed to load database");
		}
	}
}

void MainPanel::loadResources()
{
	if (m_editor->isInEditingState())
	{
		return;
	}
    QMessageBox::StandardButton btn = QMessageBox::warning(NULL, "Database will be erased", "Database will be erased. Proceed loading?", QMessageBox::Ok | QMessageBox::Cancel, QMessageBox::Ok);
    if (btn == QMessageBox::Ok)
    {
        QString name = QFileDialog::getOpenFileName(this, "Enter new resource file", "", "*.json");
        if (name.length())
        {
            sad::resource::Tree* tree = new sad::resource::Tree();
            tree->setRenderer(sad::Renderer::ref());
            tree->setStoreLinks(true);
            tree->factory()->registerResource<sad::freetype::Font>();
            
            sad::Vector<sad::resource::Error * > errors;
            errors = tree->loadFromFile(name.toStdString());
            if (errors.size())
            {
                delete tree;
                this->m_editor->reportResourceLoadingErrors(errors, name.toStdString());               
            }
            else
            {
				sad::Renderer::ref()->lockRendering();
                m_editor->cleanDatabase();
                sad::Renderer::ref()->addDatabase("", new sad::db::Database());
                this->viewDatabase();
				sad::Renderer::ref()->unlockRendering();

                sad::Renderer::ref()->removeTree("");
                sad::Renderer::ref()->addTree("", tree);
                if (ui.rtwLabelFont->filter().length() == 0)
                {
                    this->updateResourceViews();
                }
                else
                {
                    ui.rtwLabelFont->updateTree();
                    ui.rtwSpriteSprite->updateTree();
                    ui.rtwCustomObjectSchemas->updateTree();
                }
                this->toggleEditingButtons(true);
            }
        }
    }
}

void MainPanel::reloadResources()
{
	if (m_editor->isInEditingState())
	{
		return;
	}
	ReloadFileList list(this);
	if (list.exec() == QDialog::Accepted && list.selectedFile() != NULL)
	{
		m_editor->shared()->setActiveObject(NULL);
		m_editor->shared()->setSelectedObject(NULL);
		m_editor->machine()->enterState("idle");

		sad::resource::PhysicalFile* file = list.selectedFile();
		sad::Renderer::ref()->lockRendering();
		sad::Vector<sad::resource::Error*> errors = file->reload();
		sad::Renderer::ref()->unlockRendering();
		if (errors.size() == 0)
		{
            if (ui.rtwLabelFont->filter().length() == 0)
            {
                this->updateResourceViews();
            }
            else
            {
                ui.rtwLabelFont->updateTree();
                ui.rtwSpriteSprite->updateTree();
                ui.rtwCustomObjectSchemas->updateTree();
            }
		}
		else
		{
			m_editor->reportResourceLoadingErrors(errors, file->name());
		}
	}
}


void MainPanel::tabTypeChanged(int index)
{
	int oldtypeindex = (m_editor->machine()->isInState("ways")) ? 1 : 0;
	if (oldtypeindex == index)
	{
		return;
	}
	if (m_editor->isInEditingState())
	{
		invoke_blocked(ui.tabTypes, &QTabWidget::setCurrentIndex, oldtypeindex);
	}
	if (index == 0)
	{
		m_editor->tryEnterObjectEditingState();
	}
	if (index == 1)
	{
		m_editor->tryEnterWayEditingState();
	}
}

