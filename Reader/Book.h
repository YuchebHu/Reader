#pragma once

namespace Reader {
	namespace Data {
		public ref class Book sealed {
		public:

			Book();
			Book(Platform::String^ Title, Platform::String^ Tag, Platform::String^ Path);
			bool Equals(Book^ book){
				if (this->Path == book->Path)
					return true;
				return false;
			}
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
				void set(Platform::String^ value) { m_path = value; }
			}
		internal:
			Platform::String^ m_title;
			Platform::String^ m_tag;
			Platform::String^ m_path;
		};
	}
}