#pragma once
#include <windows.h>
#include <glm\glm.hpp>

namespace rendering
{
	class Window
	{
	public:
		Window(HWND* hWnd, HDC* hDC, HGLRC* hRC)
			: mHWnd(hWnd)
			, mHDC(hDC)
			, mHRC(hRC)
		{}

		void close()
		{
			wglMakeCurrent(NULL, NULL);
			wglDeleteContext(*mHRC);
			ReleaseDC(*mHWnd, *mHDC);
		}

		void display()
		{
			SwapBuffers(*mHDC);
		}

		void resize(unsigned int sizeX, unsigned int sizeY)
		{
			resize(static_cast<int>(sizeX), static_cast<int>(sizeY));
		}

		void resize(int sizeX, int sizeY)
		{
			assert(sizeX >= 0 && sizeY >= 0);
			mSize.x = sizeX;
			mSize.y = sizeY;
			MoveWindow(*mHWnd, mPosition.x, mPosition.y, sizeX, sizeY, true);
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
			ClientToScreen(*mHWnd, &pt);
			return glm::ivec2(pt.x, pt.y);
		}

		void setTitle(const std::wstring& title)
		{
			SetWindowText(*mHWnd, title.c_str());
		}

		bool hasFocus()
		{
			return *mHWnd == GetFocus();
		}

		inline const glm::uvec2& getSize() const
		{
			return mSize;
		}

		inline HWND* getHWnd()
		{
			return mHWnd;
		}

		inline HDC* getHDC()
		{
			return mHDC;
		}

		inline HGLRC* getHRC()
		{
			return mHRC;
		}
	private:
		glm::ivec2 mPosition;
		glm::uvec2 mSize;
		HWND* mHWnd;
		HDC* mHDC;
		HGLRC* mHRC;
	};
}