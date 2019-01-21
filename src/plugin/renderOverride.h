//
// Copyright 2019 Luma Pictures
//
// Licensed under the Apache License, Version 2.0 (the "Apache License")
// with the following modification you may not use this file except in
// compliance with the Apache License and the following modification to it:
// Section 6. Trademarks. is deleted and replaced with:
//
// 6. Trademarks. This License does not grant permission to use the trade
//    names, trademarks, service marks, or product names of the Licensor
//    and its affiliates, except as required to comply with Section 4(c) of
//    the License and to reproduce the content of the NOTICE file.
//
// You may obtain a copy of the Apache License at
//
//     http:#www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the Apache License with the above modification is
// distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
// KIND, either express or implied. See the Apache License for the specific
// language governing permissions and limitations under the Apache License.
//
#ifndef __MTOH_VIEW_OVERRIDE_H__
#define __MTOH_VIEW_OVERRIDE_H__

#include <pxr/imaging/glf/glew.h>
#include <pxr/pxr.h>

#include <pxr/base/tf/singleton.h>

#include <pxr/imaging/hd/engine.h>
#include <pxr/imaging/hd/renderIndex.h>
#include <pxr/imaging/hdSt/renderDelegate.h>

#include <pxr/imaging/hd/engine.h>
#include <pxr/imaging/hd/rprimCollection.h>

#include <pxr/imaging/hdx/rendererPlugin.h>
#include <pxr/imaging/hdx/taskController.h>

#include <maya/MMessage.h>
#include <maya/MString.h>
#include <maya/MViewport2Renderer.h>

#include <hdmaya/delegates/delegate.h>
#include <hdmaya/delegates/params.h>

#include "defaultLightDelegate.h"
#include "renderGlobals.h"
#include "utils.h"

#include <atomic>
#include <chrono>
#include <mutex>

PXR_NAMESPACE_OPEN_SCOPE

class MtohRenderOverride : public MHWRender::MRenderOverride {
public:
    MtohRenderOverride(const MtohRendererDescription& desc);
    ~MtohRenderOverride() override;

    static void UpdateRenderGlobals();

    MStatus Render(const MHWRender::MDrawContext& drawContext);

    void ClearHydraResources();
    void SelectionChanged();

    MString uiName() const override {
        return MString(_rendererDesc.displayName.GetText());
    }

    MHWRender::DrawAPI supportedDrawAPIs() const override;

    MStatus setup(const MString& destination) override;
    MStatus cleanup() override;

    bool startOperationIterator() override;
    MHWRender::MRenderOperation* renderOperation() override;
    bool nextRenderOperation() override;

private:
    void _InitHydraResources();
    void _SelectionChanged();
    void _DetectMayaDefaultLighting(const MHWRender::MDrawContext& drawContext);
    void _UpdateRenderGlobals();
    void _UpdateRenderDelegateOptions();

    // Callbacks
    static void _ClearHydraCallback(void* data);
    static void _TimerCallback(float, float, void* data);
    static void _ClearResourcesCallback(float, float, void* data);
    static void _SelectionChangedCallback(void* data);

    MtohRendererDescription _rendererDesc;

    std::vector<MHWRender::MRenderOperation*> _operations;
    std::vector<MCallbackId> _callbacks;
    MtohRenderGlobals _globals;

    std::mutex _convergenceMutex;
    std::chrono::system_clock::time_point _lastRenderTime;
    std::atomic<bool> _needsClear;

    HdEngine _engine;
    HdxRendererPlugin* _rendererPlugin = nullptr;
    HdxTaskController* _taskController = nullptr;
    HdRenderIndex* _renderIndex = nullptr;
    std::unique_ptr<MtohDefaultLightDelegate> _defaultLightDelegate = nullptr;
    HdxSelectionTrackerSharedPtr _selectionTracker;
    HdRprimCollection _renderCollection;
    HdRprimCollection _selectionCollection;
    GlfSimpleLight _defaultLight;

    std::vector<HdMayaDelegatePtr> _delegates;

    SdfPath _ID;

    int _currentOperation = -1;

    bool _initializedViewport = false;
    bool _preferSimpleLight = false;
    bool _hasDefaultLighting = false;
    bool _renderGlobalsHaveChanged = false;
    bool _selectionChanged = true;
    bool _isConverged = false;
};

PXR_NAMESPACE_CLOSE_SCOPE

#endif // __MTOH_VIEW_OVERRIDE_H__
