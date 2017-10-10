#pragma once
#include <windows.h>
#include <glm\glm.hpp>

namespace rendering
{
	class Window
	{
	public:
		Window(HWND* hWnd, HDC* hDC, HGLRC* hRC)
			: m_hWnd(hWnd)
			, m_hDC(hDC)
			, m_hRC(hRC)
		{}

		void close()
		{
			wglMakeCurrent(NULL, NULL);
			wglDeleteContext(*m_hRC);
			ReleaseDC(*m_hWnd, *m_hDC);
		}

		void display()
		{
			SwapBuffers(*m_hDC);
		}

		void resize(unsigned int sizeX, unsigned int sizeY)
		{
			resize(static_cast<int>(sizeX), static_cast<int>(sizeY));
		}

		void resize(int sizeX, int sizeY)
		{
			assert(sizeX >= 0 && sizeY >= 0);
			m_size.x = sizeX;
			m_size.y = sizeY;
			MoveWindow(*m_hWnd, m_position.x, m_position.y, sizeX, sizeY, true);
		}

		void setCursorPos(long posX, long posY)
		{
			POINT pt;
			pt.x = posX;
			pt.y = posY;
			//ClientToScreen(*mHWnd, &pt);
			SetCursorPos(pt.x, pt.y);
		}

		glm::ivec2 transformPointToScreen(long posX, long posY) const
		{
			POINT pt;
			pt.x = posX;
			pt.y = posY;
			ClientToScreen(*m_hWnd, &pt);
			return glm::ivec2(pt.x, pt.y);
		}

		void setTitle(const std::wstring& title)
		{
			SetWindowText(*m_hWnd, title.c_str());
		}

		bool hasFocus()
		{
			return *m_hWnd == GetFocus();
		}

		inline const glm::uvec2& getSize() const
		{
			return m_size;
		}

		inline HWND* getHWnd()
		{
			return m_hWnd;
		}

		inline HDC* getHDC()
		{
			return m_hDC;
		}

		inline HGLRC* getHRC()
		{
			return m_hRC;
		}
	private:
		glm::ivec2 m_position;
		glm::uvec2 m_size;
		HWND* m_hWnd;
		HDC* m_hDC;
		HGLRC* m_hRC;
	};
}