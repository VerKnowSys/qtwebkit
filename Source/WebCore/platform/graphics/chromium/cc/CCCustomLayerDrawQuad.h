/*
 * Copyright (C) 2011 Google Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1.  Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 * 2.  Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE AND ITS CONTRIBUTORS "AS IS" AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL APPLE OR ITS CONTRIBUTORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef CCCustomLayerDrawQuad_h
#define CCCustomLayerDrawQuad_h

#include "cc/CCDrawQuad.h"
#include <wtf/PassOwnPtr.h>

namespace WebCore {

class CCLayerImpl;

// FIXME: This class is a temporary way to access CCLayerImpl::draw. This class
// should be converted to a set of draw quads for each layer material type and
// then removed.
class CCCustomLayerDrawQuad : public CCDrawQuad {
    WTF_MAKE_NONCOPYABLE(CCCustomLayerDrawQuad);
public:
    static PassOwnPtr<CCCustomLayerDrawQuad> create(const CCSharedQuadState*, const IntRect&, CCLayerImpl*);

    CCLayerImpl* layer() const { return m_layer; }

private:
    CCCustomLayerDrawQuad(const CCSharedQuadState*, const IntRect&, CCLayerImpl*);

    CCLayerImpl* m_layer;
};

}

#endif