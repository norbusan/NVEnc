﻿// -----------------------------------------------------------------------------------------
// QSVEnc/NVEnc/VCEEnc by rigaya
// -----------------------------------------------------------------------------------------
// The MIT License
//
// Copyright (c) 2019 rigaya
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//
// --------------------------------------------------------------------------------------------

#pragma once
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <Windows.h>
#include <string>
#include <thread>
#include <mutex>
#include <vector>

struct CounterEntry {
    LUID luid;
    int pid;
    int phys;
    int eng;
    std::wstring type;
    int val;

    CounterEntry();
    int set(const wchar_t *name, DWORD value);
    void print() const;
};

class RGYGPUCounterWinEntries {
public:
    RGYGPUCounterWinEntries(const std::vector<CounterEntry> &counterEntries);
    ~RGYGPUCounterWinEntries();
    RGYGPUCounterWinEntries filter_luid(LUID luid);
    RGYGPUCounterWinEntries filter_pid(int pid);
    RGYGPUCounterWinEntries filter_phys(int phys);
    RGYGPUCounterWinEntries filter_eng(int eng);
    RGYGPUCounterWinEntries filter_type(const std::wstring &type);
    std::vector<CounterEntry> get() const;
    int sum() const;
protected:
    std::wstring tolowercase(const std::wstring &str);
    std::vector<const CounterEntry *> entries;
};


struct IWbemConfigureRefresher;
struct IWbemRefresher;
struct IWbemHiPerfEnum;
struct IWbemServices;

class RGYGPUCounterWin {
public:
    RGYGPUCounterWin();
    virtual ~RGYGPUCounterWin();

    int init();
    int refreshCounters();
    void close();

    void thread_run();
    void thread_fin();

    std::mutex &getmtx() {
        return mtxRefresh;
    }
    RGYGPUCounterWinEntries getCounters() {
        m_refreshed = false;
        return RGYGPUCounterWinEntries(counters);
    }

protected:
    int thread_func();

    std::thread thRefresh;
    std::mutex mtxRefresh;
    bool m_refreshed;
    std::vector<CounterEntry> counters;

    long m_nameHandle;
    long m_utilizationHandle;

    IWbemConfigureRefresher *pConfig;
    IWbemRefresher *pRefresher;
    IWbemHiPerfEnum *pEnum;
    IWbemServices *pNameSpace;
    bool m_abort;
};