// Copyright (c) 2003-2015 Nick Porcino, All rights reserved.
// License is MIT: http://opensource.org/licenses/MIT

#ifndef __LabSound__SupersawNode__
#define __LabSound__SupersawNode__

#include "LabSound/core/AudioContext.h"
#include "LabSound/core/AudioNode.h"
#include "LabSound/core/AudioParam.h"

namespace lab 
{
    class SupersawNode : public lab::AudioNode 
	{
        class SupersawNodeInternal;
		std::unique_ptr<SupersawNodeInternal> internalNode;

    public:

        SupersawNode(ContextRenderLock& r, float sampleRate);
        virtual ~SupersawNode();
        
		std::shared_ptr<lab::AudioParam> attack() const;
		std::shared_ptr<lab::AudioParam> decay() const;
		std::shared_ptr<lab::AudioParam> sustain() const;
		std::shared_ptr<lab::AudioParam> release() const;

        std::shared_ptr<lab::AudioParam> sawCount() const;
        std::shared_ptr<lab::AudioParam> frequency() const;
        std::shared_ptr<lab::AudioParam> detune() const;

		void noteOn(double when);
		void noteOff(ContextRenderLock&, double when);

        void update(ContextRenderLock& r); // call if sawCount is changed. CBB: update automatically

    private:

        virtual void process(ContextRenderLock&, size_t) override;
        virtual void reset(ContextRenderLock&) override { /*m_currentSampleFrame = 0;*/ }
        virtual double tailTime() const override { return 0; }
        virtual double latencyTime() const override { return 0; }
        virtual bool propagatesSilence(double now) const override;

    };
}

#endif
