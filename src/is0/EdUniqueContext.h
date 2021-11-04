#pragma once

#include <imnodes/imnodes.h>
#include "Ed.h"

class EdUniqueContext {
public:
    EdUniqueContext()
        : _context{ed::CreateEditor()}
    {
        ImNodes::CreateContext();
        ImNodes::LoadCurrentEditorStateFromIniFile("is0_node_editor.ini");
    }
    ~EdUniqueContext()
    {
        ImNodes::SaveCurrentEditorStateToIniFile("is0_node_editor.ini");
        ImNodes::DestroyContext();
        if (_context) {
            ed::DestroyEditor(_context);
        }
    }
    EdUniqueContext(const EdUniqueContext&) = delete;
    EdUniqueContext& operator=(const EdUniqueContext&) = delete;
    EdUniqueContext(EdUniqueContext&& rhs) noexcept
        : _context{rhs._context}
    {
        rhs._context = nullptr;
    };
    EdUniqueContext& operator=(EdUniqueContext&& rhs) noexcept
    {
        if (this != &rhs) {
            _context     = rhs._context;
            rhs._context = nullptr;
        }
        return *this;
    }

    ed::EditorContext&       operator*() { return *_context; }
    ed::EditorContext*       operator->() { return _context; }
    const ed::EditorContext& operator*() const { return *_context; }
    const ed::EditorContext* operator->() const { return _context; }

private:
    ed::EditorContext* _context;
};