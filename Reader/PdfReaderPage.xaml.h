#pragma once

#include "PdfReaderPage.g.h"

namespace Reader {
	[Windows::Foundation::Metadata::WebHostHidden]
	public ref class PdfReaderPage sealed {
	public:
		PdfReaderPage();
		
		void LoadDocument();
		void ViewPage();
	protected:
		virtual void OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs^ e) override;
		virtual void OnNavigatedFrom(Windows::UI::Xaml::Navigation::NavigationEventArgs^ e) override;
	private:
		Windows::Data::Pdf::PdfDocument^ pdfDocument;
		Platform::String^ tag;
		unsigned int m_currentPage = 0;
		unsigned int m_pageCount;
		Windows::Foundation::EventRegistrationToken m_sizeChangedEventRegistrationToken;
		Windows::Foundation::EventRegistrationToken m_backRequestedEventRegistrationToken;
		
		void PreviousPage(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void NextPage(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		
		
		void PdfReaderPage_BackRequested(Object^ sender, Windows::UI::Core::BackRequestedEventArgs^ args);
		void OnBackRequested();
		
		void SelectionChanged(Platform::Object^ sender, Windows::UI::Xaml::Controls::SelectionChangedEventArgs^ e);
		void CloseBook(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
	};
}