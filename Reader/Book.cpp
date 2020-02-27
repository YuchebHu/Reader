#include "pch.h"
#include "Book.h"

using namespace Reader::Data;

Book::Book() {}

Book::Book(Platform::String^ Title, Platform::String^ Tag, Platform::String^ Path) :
	m_title(Title), m_tag(Tag), m_path(Path) {}