//
// MainPage.xaml.cpp
// MainPage 类的实现。
//

#include "pch.h"
#include "MainPage.xaml.h"
#include "BookViewModel.h"
#include "DetailPage.xaml.h"


using namespace Reader;
using namespace Reader::Data;
using namespace Reader::ViewModels;

using namespace Concurrency;
using namespace Platform;
using namespace Platform::Collections;
using namespace Windows::Storage;
using namespace Windows::Storage::Pickers;
using namespace Windows::Storage::Streams;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Controls::Primitives;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Interop;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml::Media::Animation;
using namespace Windows::UI::Xaml::Navigation;


// https://go.microsoft.com/fwlink/?LinkId=402352&clcid=0x804 上介绍了“空白页”项模板

MainPage::MainPage() {

	InitializeComponent();
}

void Reader::MainPage::OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs^ e) {


	Page::OnNavigatedTo(e);
	auto tags = dynamic_cast<Vector<BookViewModel^>^>(MainListView->ItemsSource);
	if (tags == nullptr) {
		auto instance = BookDataSource::GetBookDataSource();

		m_tags = ref new Vector<BookViewModel^>();
		for (auto tag : instance->GetAllTags()) {
			m_tags->Append(BookViewModel::FromTag(tag));
		}

		MainListView->ItemsSource = m_tags;
		m_lastSelectedTag = m_tags->GetAt(0);
		currentTag = m_lastSelectedTag->Tag;
		navigationTransitionInfo = ref new DrillInNavigationTransitionInfo();
		DetailFrame->Navigate(TypeName(DetailPage::typeid), currentTag, navigationTransitionInfo);
	}

}

void Reader::MainPage::LayoutRoot_Loaded(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e) {
	MainListView->SelectedItem = m_lastSelectedTag;
}

void Reader::MainPage::MainListView_BookClick(Platform::Object^ sender, Windows::UI::Xaml::Controls::ItemClickEventArgs^ e) {
	auto clickedItem = safe_cast<BookViewModel^>(e->ClickedItem);
	if (currentTag != clickedItem->Tag) {
		m_lastSelectedTag = clickedItem;
		currentTag = m_lastSelectedTag->Tag;
		
		DetailFrame->Navigate(TypeName(DetailPage::typeid), clickedItem->Tag, navigationTransitionInfo);
	}
}

void Reader::MainPage::AddBook(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e) {
	auto picker = ref new FileOpenPicker();
	picker->FileTypeFilter->Append(".pdf");

	create_task(picker->PickSingleFileAsync()).then([this](StorageFile^ file) {
		if (file != nullptr) {
			auto book = ref new Book(file->Name, L"All Books", file->Path);
			auto instance = BookDataSource::GetBookDataSource();
			instance->InsertBook(book);
			/*auto books = ref new Vector<BookViewModel^>();
			auto TagMap = ref new Map<Platform::String^, BookViewModel^>();
			for (auto book : instance->GetAllBooks()) {
				if (book->Tag != L"All Books")
					TagMap->Insert(book->Tag, BookViewModel::FromBook(book));
			}
			books->Append(BookViewModel::FromTag(L"All Books"));
			for (auto bookView : TagMap) {
				books->Append(bookView->Value);
			}*/
	
			if (currentTag == "All Books") {
				DetailFrame->Navigate(TypeName(DetailPage::typeid), currentTag, navigationTransitionInfo);
			}
			//UpdateForVisualState(AdaptiveStates->CurrentState);
		}
		});
}

void Reader::MainPage::DisplayDeleteTagDialog() {
	DeleteText->Text = L"Do you want to delete " + m_lastSelectedTag->Tag + L"?";
	create_task(TagDeleteDialog->ShowAsync()).then([this](ContentDialogResult result) {
		if (result == ContentDialogResult::Primary) {
			auto instance = BookDataSource::GetBookDataSource();
			instance->DeleteTag(m_lastSelectedTag->Tag);
			int i = 0;
			for (auto m_tag : m_tags) {
				if (m_tag->Tag == m_lastSelectedTag->Tag) {
					break;
				}
				++i;
			}
			m_tags->RemoveAt(i);
			MainListView->ItemsSource = m_tags;
			if (currentTag == m_lastSelectedTag->Tag) {
				m_lastSelectedTag->Tag = L"All Books";
				currentTag = L"All Books";
				DetailFrame->Navigate(TypeName(DetailPage::typeid), currentTag, navigationTransitionInfo);
			}
		}
		});
}

void Reader::MainPage::DisplayRenameTagDialog() {
	RenameText->Text = m_lastSelectedTag->Tag;
	TagRenameDialog->Title = L"Rename";
	create_task(TagRenameDialog->ShowAsync()).then([this](ContentDialogResult result) {
		if (result == ContentDialogResult::Primary) {
			auto instance = BookDataSource::GetBookDataSource();
			instance->RenameTag(m_lastSelectedTag->Tag, m_rename);
			for (auto m_tag : m_tags) {
				if (m_tag->Tag == m_lastSelectedTag->Tag) {
					m_tag = BookViewModel::FromTag(m_rename);
					break;
				}
				
			}
		
			MainListView->ItemsSource = m_tags;
			m_lastSelectedTag->Tag = m_rename;
		}
		});
}

void Reader::MainPage::DisplayAddTagDialog() {
	RenameText->PlaceholderText = L"Tag";
	RenameText->Text = L"";
	TagRenameDialog->Title = L"Add Tag";
	create_task(TagRenameDialog->ShowAsync()).then([this](ContentDialogResult result) {
		if (result == ContentDialogResult::Primary) {
			auto instance = BookDataSource::GetBookDataSource();
			instance->AddTag(m_rename);
			m_tags->Append(BookViewModel::FromTag(m_rename));
			MainListView->ItemsSource = m_tags;
		}
		});
}

void Reader::MainPage::TagRightTapped(Platform::Object^ sender, Windows::UI::Xaml::Input::RightTappedRoutedEventArgs^ e) {
	m_lastSelectedTag = safe_cast<BookViewModel^>(safe_cast<TextBlock^>(e->OriginalSource)->DataContext);
	if (m_lastSelectedTag->Tag != L"All Books")
		FlyoutBase::ShowAttachedFlyout((FrameworkElement^)sender);
}

void Reader::MainPage::TextChanged(Platform::Object^ sender, Windows::UI::Xaml::Controls::TextChangedEventArgs^ e) {
	TextBox^ tb = safe_cast<TextBox^>(sender);
	m_rename = tb->Text;
}

void Reader::MainPage::RenameTag(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e) {
	DisplayRenameTagDialog();
}

void Reader::MainPage::AddTag(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e) {
	DisplayAddTagDialog();
}

void Reader::MainPage::DeleteTag(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e) {
	DisplayDeleteTagDialog();
}

void Reader::MainPage::ToggleButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e) {
	Splitter->IsPaneOpen = !Splitter->IsPaneOpen;
}
