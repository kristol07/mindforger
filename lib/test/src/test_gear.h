/*
 test_gear.h     MindForger thinking notebook

 Copyright (C) 2016-2019 Martin Dvorak <martin.dvorak@mindforger.com>

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 as published by the Free Software Foundation; either version 2
 of the License, or (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program. If not, see <http://www.gnu.org/licenses/>.
*/
#ifndef M8R_TEST_GEAR_H
#define M8R_TEST_GEAR_H

#include <string>
#include <map>

#include "../../src/model/outline.h"
#include "../../src/representations/html/html_outline_representation.h"

namespace m8r {

class DummyHtmlColors : public m8r::HtmlColorsRepresentation {
private:
    std::string textColor;
    std::string backgroundColor;
public:
    virtual std::string& getHtmlTextColor() { return textColor; }
    virtual std::string& getHtmlBackgroundColor() { return backgroundColor; }
};

void printOutlineNotes(Outline* o);
void printLexemType(MarkdownLexemType type);
void printLexems(const std::vector<MarkdownLexem*>& lexems);
void printAst(const std::vector<MarkdownAstNodeSection*>* ast);

void createEmptyRepository(std::string& repositoryDir, std::map<std::string,std::string>& pathToContent);

std::string platformSpecificPath(const char *path);

}
#endif // M8R_TEST_GEAR_H
