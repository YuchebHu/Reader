#include "pch.h"
#include "BookViewModel.h"

using namespace Reader::Data;
using namespace Reader::ViewModels;
using namespace Windows::Globalization::DateTimeFormatting;

BookViewModel^ BookViewModel::FromBook(::Reader::Data::Book^ book) {
	auto viewModel = ref new BookViewModel();

	viewModel->m_title = book->Title;
	viewModel->m_tag = book->Tag;
	viewModel->m_path = book->Path;

	return viewModel;
}

BookViewModel^ Reader::ViewModels::BookViewModel::FromTag(Platform::String^ Tag) {
	auto bookView = ref new BookViewModel();
	bookView->m_tag = Tag;
	return bookView;
}

BookViewModel::BookViewModel() {}


