#include "pch.h"
#include "PdfReaderPage.xaml.h"
#include "DetailPage.xaml.h"
#include <experimental/resumable>
#include <ppltasks.h>
#include <pplawait.h>

using namespace Reader;

using namespace concurrency;
using namespace Platform;
using namespace Platform::Collections;
using namespace Windows::Data::Pdf;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::Storage;
using namespace Windows::Storage::Pickers;
using namespace Windows::Storage::Streams;
using namespace Windows::UI::Core;
using namespace Windows::UI::ViewManagement;
using namespace Windows::System;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Interop;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Media::Imaging;
using namespace Windows::UI::Xaml::Media::Animation;
using namespace Windows::UI::Xaml::Navigation;


Reader::PdfReaderPage::PdfReaderPage() : m_sizeChangedEventRegistrationToken(), m_backRequestedEventRegistrationToken() {
	InitializeComponent();
}

void Reader::PdfReaderPage::OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs^ e) {
	Page::OnNavigatedTo(e);
	auto backStack = Frame->BackStack;
	auto backStackCount = backStack->Size;
	pdfDocument = safe_cast<PdfDocument^>(e->Parameter);
	
	CurrentPage->Items->Clear();
	LoadDocument();
	ViewPage();
	SystemNavigationManager^ systemNavigationManager = SystemNavigationManager::GetForCurrentView();
	m_backRequestedEventRegistrationToken =
		systemNavigationManager->BackRequested += ref new EventHandler<BackRequestedEventArgs^>(this, &PdfReaderPage::PdfReaderPage_BackRequested);
	systemNavigationManager->AppViewBackButtonVisibility = AppViewBackButtonVisibility::Collapsed;
}

void Reader::PdfReaderPage::OnNavigatedFrom(Windows::UI::Xaml::Navigation::NavigationEventArgs^ e) {
	Page::OnNavigatedFrom(e);
	SystemNavigationManager^ systemNavigationManager = SystemNavigationManager::GetForCurrentView();
	systemNavigationManager->BackRequested -= m_backRequestedEventRegistrationToken;
	
}

void Reader::PdfReaderPage::PdfReaderPage_BackRequested(Object^ sender, BackRequestedEventArgs^ args) {
	args->Handled = true;
	OnBackRequested();
}

void Reader::PdfReaderPage::OnBackRequested() {
	if (Frame->CanGoBack)
		Frame->GoBack(ref new DrillInNavigationTransitionInfo());
}

void Reader::PdfReaderPage::LoadDocument() {
	m_pageCount = pdfDocument->PageCount;

	for (unsigned int i = 0; i < m_pageCount; ++i) {
		auto item = ref new ComboBoxItem();
		item->Content = (i + 1).ToString();
		CurrentPage->Items->Append(item);
	}

}

void Reader::PdfReaderPage::ViewPage() {
	//pdfDocument = tempReader().get();

	PdfPage^ page = pdfDocument->GetPage(m_currentPage);
	auto stream = ref new InMemoryRandomAccessStream();
	IAsyncAction^ renderAction;
	auto options1 = ref new PdfPageRenderOptions();
	options1->BackgroundColor = Windows::UI::Colors::Beige;
	options1->DestinationHeight = static_cast<unsigned int>(page->Size.Height / 0.8);
	options1->DestinationWidth = static_cast<unsigned int>(page->Size.Width / 0.8);
	renderAction = page->RenderToStreamAsync(stream, options1);
	renderAction = page->RenderToStreamAsync(stream);
	Output->Source = nullptr;
	create_task(renderAction).then([this, stream]() {
		auto src = ref new BitmapImage();
		Output->Source = src;
		return  create_task(src->SetSourceAsync(stream));
		});
}

void Reader::PdfReaderPage::NextPage(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e) {
	if (m_currentPage < m_pageCount - 1) {
		++m_currentPage;
		CurrentPage->SelectedIndex = m_currentPage;
		//ViewPage();
	}
}

void Reader::PdfReaderPage::PreviousPage(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e) {
	if (m_currentPage > 0) {
		--m_currentPage;
		CurrentPage->SelectedIndex = m_currentPage;
		//ViewPage();
	}
}

void Reader::PdfReaderPage::SelectionChanged(Platform::Object^ sender, Windows::UI::Xaml::Controls::SelectionChangedEventArgs^ e) {
	auto s = safe_cast<ComboBox^>(sender);
	m_currentPage = static_cast<unsigned int>(s->SelectedIndex);
	ViewPage();
}


void Reader::PdfReaderPage::CloseBook(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e) {
	OnBackRequested();
	
}
