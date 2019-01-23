#ifndef _X_THREAD_GUARD_HPP
#define _X_THREAD_GUARD_HPP

#include <thread>


class XThreadGuard
{
public:
	template <typename _Fn,typename... _Args>
	XThreadGuard(_Fn&& _Fx, _Args&&... _Ax) : m_done(false)
	{
		m_th = std::thread(std::forward<_Fn>(_Fx),std::forward<_Args>(_Ax)...);
	}
	~XThreadGuard(){ join(); }

	XThreadGuard(const XThreadGuard &) = delete;
	XThreadGuard& operator=(const XThreadGuard &) = delete;


	void join()
	{
		if (m_th.joinable())
			m_th.join();
		m_done = true;
	}
private:
	std::thread m_th;
	bool m_done;
};

#endif //_X_THREAD_GUARD_HPP
