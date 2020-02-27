#pragma once


namespace Reader {
	namespace ViewModels {
		public ref class BookViewModel sealed {
		public:
			static BookViewModel^ FromBook(::Reader::Data::Book^ book);

			static BookViewModel^ FromTag(Platform::String^ Tag);

			property Platform::String^ Title {
				Platform::String^ get() { return m_title; }
				void set(Platform::String^ value) { m_title = value; }
			}
			
			property Platform::String^ Tag {
				Platform::String^ get() { return m_tag; }
				void set(Platform::String^ value) { m_tag = value; }
			}

			property Platform::String^ Path {
				Platform::String^ get() { return m_path; }
				void set(Platform::String^ value) { m_path= value; }
			}

		private:
			BookViewModel();

			Platform::String^ m_title;
			Platform::String^ m_tag;
			Platform::String^ m_path;
		};
	}
}