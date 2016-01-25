#include "ReportWriter.h"

ReportWriter::ReportWriter(const char* fileName) {
	_file = fopen(fileName, "w");
	if (_file) {
		fprintf(_file, "<!doctype html>\n<html>\n<head>\n<link rel = 'stylesheet' href = 'report.css'></head><body>\n");
	}
	_open = true;
}

ReportWriter::~ReportWriter() {
	if (_file) {
		fprintf(_file, "</body></html>\n");
		fclose(_file);
	}
}
void ReportWriter::addHeader(const char* header) const {
	assert(_open);
	fprintf(_file, "<h1>%s</h1>\n",header);
}

void ReportWriter::startBox(const char* header) const {
	assert(_open);
	fprintf(_file, "<div class = 'panel'>\n<div class = 'panel-heading'><h1>%s</h1></div>\n", header);
}

void ReportWriter::endBox() const {
	assert(_open);
	fprintf(_file, "</div>\n");
}

void ReportWriter::addSubHeader(const char* header) const {
	assert(_open);
	fprintf(_file, "<h2>%s</h2>\n", header);
}

void ReportWriter::startTable(const char** columnNames, int num) const {
	assert(_open);
	fprintf(_file, "<table id = 'box-table-a'><thead>  <tr>");
	for (int i = 0; i < num; ++i) {
		fprintf(_file, "    <th scope='col'>%s</th>\n", columnNames[i]);
	}
	fprintf(_file, "  </tr></thead><tbody>\n");
}

void ReportWriter::startRow() const {
	assert(_open);
	fprintf(_file, "  <tr>\n");
}

void ReportWriter::addCell(int v) const {
	assert(_open);
	fprintf(_file, "    <td>%d</td>\n",v);
}

void ReportWriter::addCell(uint32 v) const {
	assert(_open);
	fprintf(_file, "    <td>%d</td>\n", v);
}

void ReportWriter::addCell(const v2& v) const {
	assert(_open);
	fprintf(_file, "    <td>x: %g  y: %g</td>\n", v.x,v.y);
}

void ReportWriter::addCell(float v) const {
	assert(_open);
	fprintf(_file, "    <td>%g</td>\n", v);
}

void ReportWriter::addCell(const char* v) const {
	assert(_open);
	fprintf(_file, "    <td>%s</td>\n", v);
}

void ReportWriter::addCell(int ident,const char* v) const {
	assert(_open);
	fprintf(_file, "    <td>");
	for (int i = 0; i < ident; ++i) {
		fprintf(_file, "&nbsp;");
	}
	fprintf(_file, "%s</td>\n", v);
}

void ReportWriter::addCell(bool v) const {
	assert(_open);
	if (v) {
		fprintf(_file, "    <td>true</td>\n");
	}
	else {
		fprintf(_file, "    <td>false</td>\n");
	}
}

void ReportWriter::endRow() const {
	assert(_open);
	fprintf(_file, "  </tr>\n");
}

void ReportWriter::endTable() const {
	assert(_open);
	fprintf(_file, "</tbody>\n</table>\n");
}
