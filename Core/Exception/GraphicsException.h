// GraphicsException.h

#pragma once

#define GRAPHIC_EXCEPT_INFO(hr) DxGraphic::HRException(__LINE__, __FILE__, (hr))
#define GRAPHIC_FAILED(hr) if (FAILED(hr)) throw DxGraphic::HRException(__LINE__, __FILE__, (hr))

#ifndef NDEBUG
#define GRAPHIC_EXCEPT(hr) DxGraphic::HRException(__LINE__, __FILE__, (hr), infoManager.GetMessages())
#define GRAPHIC_THROW_INFO(hr) infoManager.Set(); if (FAILED(hr)) throw GRAPHIC_EXCEPT_INFO(hr)
#define GRAPHIC_REMOVE_EXCEPT(hr) DxGraphic::RemoveException(__LINE__, __FILE__, (hr), infoManager.GetMessages())
#define GRAPHIC_THROW_INFO_ONLY(hr) infoManager.Set(); (hr); {auto v = infoManager.GetMessages(); if(!v.empty())  {throw DxGraphic::InfoException(__LINE__, __FILE__, v); } }
#else
#define GRAPHIC_EXCEPT(hr) DxGraphic::HRException(__LINE__, __FILE__, (hr))
#define GRAPHIC_THROW_INFO(hr) GRAPHIC_EXCEPT_INFO(hr)
#define GRAPHIC_REMOVE_EXCEPT(hr) DxGraphic::RemoveException(__LINE__, __FILE__, (hr))
#define GRAPHIC_INFO_ONLY(hr) (hr)
#endif

#ifdef NDEBUG
#define CREATEINFOMANAGER(graphic) HRESULT hr;
#else
#define CREATEINFOMANAGER(graphic) HRESULT hr; ExceptionInfo& infoManager = GetInfoManager((graphic));
#endif