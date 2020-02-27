//
// MainPage.xaml.h
// MainPage 类的声明。
//

#pragma once

#include "MainPage.g.h"

namespace Reader {

	/// <summary>
	/// 可用于自身或导航至 Frame 内部的空白页。
	/// </summary>
	public ref class MainPage sealed {
	public:
		MainPage();
		
	protected:
		virtual void OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs^ e) override;

	private:
		::Reader::ViewModels::BookViewModel^ m_lastSelectedBook;
		::Reader::ViewModels::BookViewModel^ m_lastSelectedTag;
		Platform::String^ currentTag;
		Platform::String^ m_rename;
		Windows::UI::Xaml::Media::Animation::NavigationTransitionInfo^ navigationTransitionInfo;
		Platform::Collections::Vector<::Reader::ViewModels::BookViewModel^>^ m_tags;

		void LayoutRoot_Loaded(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void MainListView_BookClick(Platform::Object^ sender, Windows::UI::Xaml::Controls::ItemClickEventArgs^ e);
	
	private:
		void DisplayDeleteTagDialog();
		void DisplayRenameTagDialog();
		void DisplayAddTagDialog();

		void AddBook(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void TagRightTapped(Platform::Object^ sender, Windows::UI::Xaml::Input::RightTappedRoutedEventArgs^ e);
		void TextChanged(Platform::Object^ sender, Windows::UI::Xaml::Controls::TextChangedEventArgs^ e);

		void RenameTag(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void AddTag(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void DeleteTag(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void ToggleButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
	};
}
