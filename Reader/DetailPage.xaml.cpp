#include "pch.h"
#include "DetailPage.xaml.h"
#include "Book.h"
#include "BookViewModel.h"
#include "PdfReaderPage.xaml.h"

using namespace Reader;
using namespace Reader::Data;
using namespace Reader::ViewModels;

using namespace Platform;
using namespace Concurrency;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::Storage;
using namespace Windows::Data::Pdf;
using namespace Windows::UI::Core;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Controls::Primitives;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Interop;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml::Media::Animation;
using namespace Windows::UI::Xaml::Navigation;


//DependencyProperty^ DetailPage::s_bookProperty = nullptr;

Reader::DetailPage::DetailPage() : m_sizeChangedEventRegistrationToken(), m_backRequestedEventRegistrationToken() {
	InitializeComponent();
}

void Reader::DetailPage::OnNavigatedTo(NavigationEventArgs^ e) {
	Page::OnNavigatedTo(e);
	currentTag = safe_cast<Platform::String^>(e->Parameter);
	DetailContentPresenter->ItemsSource = Items::get();
	pdf = nullptr;

	SystemNavigationManager^ systemNavigationManager = SystemNavigationManager::GetForCurrentView();
	m_backRequestedEventRegistrationToken =
		systemNavigationManager->BackRequested += ref new EventHandler<BackRequestedEventArgs^>(this, &DetailPage::DetailPage_BackRequested);

}

void DetailPage::OnNavigatedFrom(NavigationEventArgs^ e) {
	Page::OnNavigatedFrom(e);

	SystemNavigationManager^ systemNavigationManager = SystemNavigationManager::GetForCurrentView();
	systemNavigationManager->BackRequested -= m_backRequestedEventRegistrationToken;

}

void Reader::DetailPage::RegisterDependencyProperties() {

}

void Reader::DetailPage::DetailPage_BackRequested(Object^ sender, BackRequestedEventArgs^ args) {
	args->Handled = true;
	OnBackRequested();
}

void DetailPage::OnBackRequested() {
	Frame->GoBack(ref new DrillInNavigationTransitionInfo());
}

IIterable<BookViewModel^>^ DetailPage::Items::get() {
	auto BVM = ref new Platform::Collections::Vector<BookViewModel^>();
	auto BDS = BookDataSource::GetBookByTag(currentTag);
	for (auto bds : BDS) {
		BVM->Append(BookViewModel::FromBook(bds));
	}

	return BVM;
}

void Reader::DetailPage::DeleteBook(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e) {
	DisplayDeleteBookDialog();
}

void Reader::DetailPage::DisplayDeleteBookDialog() {
	DeleteText->Text = L"Do you want to delete " + m_lastSelectedBook->Title + L"?";
	create_task(DeleteDialog->ShowAsync()).then([this](ContentDialogResult result) {
		if (result == ContentDialogResult::Primary) {
			auto instance = BookDataSource::GetBookDataSource();

			instance->DeleteBook(ref new Book(m_lastSelectedBook->Title, m_lastSelectedBook->Tag, m_lastSelectedBook->Path));
			DetailContentPresenter->ItemsSource = Items::get();
		}
		});
}

void Reader::DetailPage::RenameBook(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e) {
	DisplayRenameBookDialog();
}

void Reader::DetailPage::DisplayRenameBookDialog() {
	RenameText->Text = m_lastSelectedBook->Title;
	RenameDialog->Title = L"Rename";
	create_task(RenameDialog->ShowAsync()).then([this](ContentDialogResult result) {
		if (result == ContentDialogResult::Primary) {
			auto instance = BookDataSource::GetBookDataSource();
			instance->RenameBook(ref new Book(m_lastSelectedBook->Title, m_lastSelectedBook->Tag, m_lastSelectedBook->Path), Rename);
			DetailContentPresenter->ItemsSource = Items::get();
		}
		});
}

void Reader::DetailPage::BookRightTapped(Platform::Object^ sender, Windows::UI::Xaml::Input::RightTappedRoutedEventArgs^ e) {
	m_lastSelectedBook = safe_cast<BookViewModel^>(safe_cast<TextBlock^>(e->OriginalSource)->DataContext);
	if (BookDataSource::GetAllTags()->Size != 1) {
		auto subMenu = ref new MenuFlyoutSubItem();
		subMenu->Text = L"Tag";
		subMenu->Icon = ref new SymbolIcon(Symbol::Tag);
		for (auto tag : BookDataSource::GetAllTags()) {
			if (!tag->Equals("All Books")) {
				auto Tag = ref new MenuFlyoutItem();
				Tag->Text = tag;
				Tag->Click += ref new RoutedEventHandler(this, &DetailPage::ChangeTag);
				subMenu->Items->Append(Tag);
			}
		}
		if (BookMenu->Items->Size == 4) {
			BookMenu->Items->InsertAt(2, subMenu);
		}
		else {
			BookMenu->Items->SetAt(2, subMenu);
		}
	}
	else {
		if (BookMenu->Items->Size == 5) {
			BookMenu->Items->RemoveAt(2);
		}
	}

	FlyoutBase::SetAttachedFlyout((FrameworkElement^)sender, BookMenu);
	FlyoutBase::ShowAttachedFlyout((FrameworkElement^)sender);
	LoadFile();
}

void Reader::DetailPage::TextChanged(Platform::Object^ sender, Windows::UI::Xaml::Controls::TextChangedEventArgs^ e) {
	TextBox^ tb = safe_cast<TextBox^>(sender);
	Rename = tb->Text;
}

void Reader::DetailPage::ChangeTag(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e) {
	auto FlyoutItem = safe_cast<MenuFlyoutItem^>(e->OriginalSource);
	auto Tag = FlyoutItem->Text;
	auto instance = BookDataSource::GetBookDataSource();
	instance->ChangeBookTag(ref new Book(m_lastSelectedBook->Title, m_lastSelectedBook->Tag, m_lastSelectedBook->Path), Tag);
	if (currentTag != "All Books") {
		DetailContentPresenter->ItemsSource = Items::get();
	}
}

void Reader::DetailPage::OpenFile(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e) {
	pdf = nullptr;
	auto m = concurrency::create_task(StorageFile::GetFileFromPathAsync(m_lastSelectedBook->Path)).then([this](StorageFile^ file) {
		return concurrency::create_task(PdfDocument::LoadFromFileAsync(file));
		}).then([this](task<Windows::Data::Pdf::PdfDocument^> t) {
			PDF::set(t.get());
			this->Frame->Navigate(TypeName(PdfReaderPage::typeid), pdf, ref new DrillInNavigationTransitionInfo());
			pdf = nullptr;
			});



}

void Reader::DetailPage::LoadFile() {

}

void Reader::DetailPage::BookDoubleTapped(Platform::Object^ sender, Windows::UI::Xaml::Input::DoubleTappedRoutedEventArgs^ e) {
	m_lastSelectedBook = safe_cast<BookViewModel^>(safe_cast<TextBlock^>(e->OriginalSource)->DataContext);
	OpenFile(sender, e);
}

