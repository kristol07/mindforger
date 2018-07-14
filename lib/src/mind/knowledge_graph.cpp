/*
 knowledge_graph.cpp     MindForger thinking notebook

 Copyright (C) 2016-2018 Martin Dvorak <martin.dvorak@mindforger.com>

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 as published by the Free Software Foundation; either version 2
 of the License, or (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program. If not, see <http://www.gnu.org/licenses/>.
*/
#include "knowledge_graph.h"

namespace m8r {

using namespace std;

/*
 * Knowledge SUB graph
 */

KnowledgeSubGraph::KnowledgeSubGraph(KnowledgeGraphNode* centralNode, int limit)
    : centralNode(centralNode), limit(limit)
{
    this->count = limit-1;
}

/*
 * Knowledge graph
 */

KnowledgeGraph::KnowledgeGraph(
        Mind* mind,
        long unsigned coreColor,
        long unsigned outlinesColor,
        long unsigned notesColor
        )
    : mind{mind}
{
    mindNode = new KnowledgeGraphNode{KnowledgeGraphNodeType::MIND, "MIND"};
    tagsNode = new KnowledgeGraphNode{KnowledgeGraphNodeType::TAGS, "tags"};
    outlinesNode = new KnowledgeGraphNode{KnowledgeGraphNodeType::OUTLINES, "outlines"};
    notesNode = new KnowledgeGraphNode{KnowledgeGraphNodeType::NOTES, "notes"};
    limboNode = new KnowledgeGraphNode{KnowledgeGraphNodeType::LIMBO, "limbo"};
    stencilsNode = new KnowledgeGraphNode{KnowledgeGraphNodeType::STENCILS, "stencils"};

    this->coreColor = coreColor;
    this->outlinesColor = outlinesColor;
    this->notesColor = notesColor;
}

KnowledgeGraph::~KnowledgeGraph()
{
    delete mindNode;
    delete tagsNode;
    delete outlinesNode;
    delete notesNode;
    delete limboNode;
    delete stencilsNode;
}

KnowledgeGraphNode* KnowledgeGraph::getNode(KnowledgeGraphNodeType type)
{
    switch(type) {
    case KnowledgeGraphNodeType::MIND:
        return mindNode;
    case KnowledgeGraphNodeType::OUTLINES:
        return outlinesNode;
    case KnowledgeGraphNodeType::NOTES:
        return notesNode;
    case KnowledgeGraphNodeType::TAGS:
        return tagsNode;
    case KnowledgeGraphNodeType::STENCILS:
        return stencilsNode;
    case KnowledgeGraphNodeType::LIMBO:
        return limboNode;
    case KnowledgeGraphNodeType::OUTLINE:
    case KnowledgeGraphNodeType::NOTE:
    case KnowledgeGraphNodeType::TAG:
    case KnowledgeGraphNodeType::STENCIL:
        return nullptr;
    }

    return nullptr;
}

// TODO this method leaks a lot - knowledge graph nodes
void KnowledgeGraph::getRelatedNodes(KnowledgeGraphNode* centralNode, KnowledgeSubGraph& subgraph)
{
    subgraph.clear();

    // significant ontology things
    if(centralNode == mindNode) {
        subgraph.setCentralNode(mindNode);

        subgraph.addChild(tagsNode);
        subgraph.addChild(outlinesNode);
        subgraph.addChild(notesNode);
        subgraph.addChild(stencilsNode);
        subgraph.addChild(limboNode);

        return;
    } else if(centralNode == outlinesNode) {
        subgraph.setCentralNode(outlinesNode);

        const vector<Outline*>& outlines = mind->getOutlines();
        if(outlines.size()) {
            KnowledgeGraphNode* k;
            for(Outline* o:outlines) {
                // TODO: reuse and delete - map<Thing*,Node*>
                k = new KnowledgeGraphNode{KnowledgeGraphNodeType::OUTLINE, o->getName(), outlinesColor};
                k->setThing(o);
                subgraph.addChild(k);
            }
        }

        subgraph.addParent(mindNode);

        return;
    } else if(centralNode == notesNode) {
        subgraph.setCentralNode(notesNode);

        // IMPROVE limit maximum number of Ns to be rendered - avoid MF trashing when rendering 1M of nodes
        vector<Note*> notes{};
        mind->getAllNotes(notes);
        if(notes.size()) {
            KnowledgeGraphNode* k;
            for(Note* n:notes) {
                // TODO: reuse and delete - map<Thing*,Node*>
                k = new KnowledgeGraphNode{KnowledgeGraphNodeType::NOTE, n->getName(), notesColor};
                k->setThing(n);
                subgraph.addChild(k);
            }
        }

        subgraph.addParent(mindNode);

        return;
    } else if(centralNode == tagsNode) {
        subgraph.setCentralNode(tagsNode);

        std::vector<const Tag*>& tags = mind->getTags().values();
        KnowledgeGraphNode* n;
        for(const Tag* t:tags) {
            // TODO: reuse and delete - map<Thing*,Node*>
            n = new KnowledgeGraphNode{KnowledgeGraphNodeType::TAG, t->getName(), t->getColor().asLong()};
            subgraph.addChild(n);
        }

        subgraph.addParent(mindNode);

        return;
    } else if(centralNode == stencilsNode) {
        subgraph.setCentralNode(stencilsNode);

        subgraph.addParent(mindNode);

        return;
    } else if(centralNode == limboNode) {
        subgraph.setCentralNode(limboNode);

        subgraph.addParent(mindNode);

        return;
    }

    // things by type
    if(centralNode->getType() == KnowledgeGraphNodeType::OUTLINE) {
        subgraph.setCentralNode(centralNode);

        Outline* o = static_cast<Outline*>(centralNode->getThing());
        KnowledgeGraphNode* k;
        for(Note* n:o->getNotes()) {
            // TODO: reuse and delete - map<Thing*,Node*>
            k = new KnowledgeGraphNode{KnowledgeGraphNodeType::NOTE, n->getName(), notesColor};
            k->setThing(n);
            subgraph.addChild(k);
        }

        const std::vector<const Tag*>* tags = o->getTags();
        for(const Tag* t:*tags) {
            // TODO: reuse and delete - map<Thing*,Node*>
            k = new KnowledgeGraphNode{KnowledgeGraphNodeType::TAG, t->getName(), t->getColor().asLong()};
            subgraph.addChild(k);
        }

        subgraph.addParent(outlinesNode);

        return;
    } else if(centralNode->getType() == KnowledgeGraphNodeType::NOTE) {
        subgraph.setCentralNode(centralNode);

        Note* n = static_cast<Note*>(centralNode->getThing());
        KnowledgeGraphNode* k;
        k = new KnowledgeGraphNode{KnowledgeGraphNodeType::OUTLINE, n->getOutline()->getName(), outlinesColor};
        k->setThing(n->getOutline());
        subgraph.addParent(k);

        const std::vector<const Tag*>* tags = n->getTags();
        for(const Tag* t:*tags) {
            // TODO: reuse and delete - map<Thing*,Node*>
            k = new KnowledgeGraphNode{KnowledgeGraphNodeType::TAG, t->getName(), t->getColor().asLong()};
            subgraph.addChild(k);
        }

        subgraph.addParent(notesNode);

        return;
    } else if(centralNode->getType() == KnowledgeGraphNodeType::TAG) {
        subgraph.setCentralNode(centralNode);

        vector<const Tag*> tags{};
        tags.push_back(mind->ontology().findOrCreateTag(centralNode->getName()));
        // Os
        vector<Outline*> outlines{};
        mind->findOutlinesByTags(tags, outlines);
        if(outlines.size()) {
            KnowledgeGraphNode* k;
            for(Outline* o:outlines) {
                // TODO: reuse and delete - map<Thing*,Node*>
                k = new KnowledgeGraphNode{KnowledgeGraphNodeType::OUTLINE, o->getName(), outlinesColor};
                k->setThing(o);
                subgraph.addChild(k);
            }
        }

        // Ns
        vector<Note*> notes{};
        mind->findNotesByTags(tags, notes);
        if(notes.size()) {
            KnowledgeGraphNode* k;
            for(Note* n:notes) {
                // TODO: reuse and delete - map<Thing*,Node*>
                k = new KnowledgeGraphNode{KnowledgeGraphNodeType::NOTE, n->getName(), notesColor};
                k->setThing(n);
                subgraph.addChild(k);
            }
        }

        subgraph.addParent(tagsNode);

        return;
    } else if(centralNode->getType() == KnowledgeGraphNodeType::STENCIL) {
        subgraph.setCentralNode(centralNode);

        subgraph.addParent(stencilsNode);

        return;
    }
}

} // m8r namespace
