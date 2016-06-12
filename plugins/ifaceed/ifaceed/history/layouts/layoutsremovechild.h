/*! \file layoutsremovechild.h

    Describes a command of removing child from grid's cell
 */
#pragma once
#include "../command.h"
#include "layouts/grid.h"


namespace history
{

namespace layouts
{
/*! A command, which must be added, when user removes child from cell
    to grid's cell
 */
class RemoveChild: public history::Command
{
public:
    /*! Constructs new command for node
        \param[in] d a node
        \param[in] row a row for node
        \param[in] column a column for node
        \param[in] pos a position of child in parent cell
        \param[in] node a node, that was removed
    */
    RemoveChild(
        sad::layouts::Grid* d,
        size_t row,
        size_t column,
        size_t pos,
        sad::SceneNode* node
    );
    /*! Erases link to a node
     */
    virtual ~RemoveChild();
    /*! Applies changes, described in command
        \param[in] ob an observer for looking for command
     */
    virtual void commit(core::Editor * ob = NULL);
    /*! Reverts changes, described in command
        \param[in] ob an observer for looking for command
     */
    virtual void rollback(core::Editor * ob = NULL);
    /*! Commits a change without updating UI. In that case,
        it just removes a child from cell, updating all other stuff

        \param[in] e editor
     */
    virtual void commitWithoutUpdatingUI(core::Editor * e);
protected:
    /*! Really performs commit
        \param[in] ob object of editor
     */
    void _commit(core::Editor* ob);
    /*! Really performs rolling back
        \param[in] ob object of editor
     */
    void _rollback(core::Editor* ob);
    /*! A real implementation for commiting without updating UI
        \param[in] e editor
     */
    void _commitWithoutUpdatingUI(core::Editor * e);
    /*! A node, whose name is changed
     */
    sad::layouts::Grid * m_grid;
    /*! A row of node
     */
    size_t m_row;
    /*! A column of node
     */
    size_t m_column;
    /*! A position for removed child
     */
    size_t m_position;
    /*! A  removed child
     */
    sad::SceneNode* m_node;
};

}

}
