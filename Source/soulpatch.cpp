//      _____ _____ _____ __
//     |   __|     |  |  |  |        Auto-generated C++
//     |__   |  |  |  |  |  |__      SOUL Version 0.9.0
//     |_____|_____|_____|_____|     https://soul.dev
//

#ifndef JUCE_AUDIO_PROCESSORS_H_INCLUDED
 #error "This file is designed to be included inside a file in a JUCE project, so that the module headers have already been included before it"
#endif

#ifndef SOUL_HEADER_INCLUDED_141445920
 #error "This file is designed to be included inside a file where its corresponding auto-generated header has already been included"
#endif

#ifndef SOUL_CPP_ASSERT
 #define SOUL_CPP_ASSERT(x)
#endif

//==============================================================================
// Generated from graph 'Level', source file: Level.soulpatch
//
class SOUL_Level
{
public:
    SOUL_Level() = default;
    ~SOUL_Level() = default;

    //==============================================================================
    static constexpr uint32_t maxBlockSize = 512;
    template <typename Type, int size> struct Vector;
    template <typename Type, int size> struct FixedArray;
    template <typename Type> struct DynamicArray;

    static constexpr const char*  UID           = "callum.level";
    static constexpr const char*  name          = "Level";
    static constexpr const char*  description   = "simple level";
    static constexpr const char*  category      = "effect";
    static constexpr const char*  manufacturer  = "soul-web-playground";
    static constexpr const char*  version       = "0.1";
    static constexpr const char*  URL           = "";
    static constexpr bool         isInstrument  = false;
    static constexpr const char*  manifest      = "{\n"
    "    \"soulPatchV1\": {\n"
    "        \"ID\": \"callum.level\",\n"
    "        \"version\": \"0.1\",\n"
    "        \"name\": \"Level\",\n"
    "        \"description\": \"simple level\",\n"
    "        \"category\": \"effect\",\n"
    "        \"manufacturer\": \"soul-web-playground\",\n"
    "        \"isInstrument\": false,\n"
    "        \"source\": [\n"
    "            \"level.soul\"\n"
    "        ]\n"
    "    }\n"
    "}";

    //==============================================================================
    struct _RenderStats;
    struct _SparseStreamStatus;
    struct _Event_in_f32_1;
    struct _Stream_in_vec_2_f32_512;
    struct _Stream_out_vec_2_f32_512;
    struct soul__gain__SmoothedGainParameter___for__root__Level_smoothedGain___State;
    struct soul__gain__DynamicGain___for__root__Level_gainProcessor___State;
    struct _State;
    struct soul__gain__SmoothedGainParameter___for__root__Level_smoothedGain___IO;
    struct soul__gain__DynamicGain___for__root__Level_gainProcessor___IO;
    struct StringLiteral;

    //==============================================================================
    // The following methods provide basic initialisation and control for the processor

    void init (double newSampleRate, int sessionID)
    {
        memset (&state, 0, sizeof (state));
        sampleRate = newSampleRate;
        _initialise (state, sessionID);
        initialisedState = state;
    }

    void reset() noexcept
    {
        state = initialisedState;
    }

    uint32_t getNumXRuns() noexcept
    {
        return static_cast<uint32_t> (_get_num_xruns (state));
    }

    //==============================================================================
    // These classes and functions provide a high-level rendering method that
    // presents the processor as a set of standard audio and MIDI channels.

    static constexpr uint32_t numAudioInputChannels  = 2;
    static constexpr uint32_t numAudioOutputChannels = 2;

    struct MIDIMessage
    {
        uint32_t frameIndex = 0;
        uint8_t byte0 = 0, byte1 = 0, byte2 = 0;
    };

    struct MIDIMessageArray
    {
        const MIDIMessage* messages = nullptr;
        uint32_t numMessages = 0;
    };

    template <typename FloatType = float>
    struct RenderContext
    {
        std::array<const FloatType*, 2> inputChannels;
        std::array<FloatType*, 2> outputChannels;
        MIDIMessageArray  incomingMIDI;
        uint32_t          numFrames = 0;
    };

    //==============================================================================
    template <typename FloatType>
    void render (RenderContext<FloatType> context)
    {
        uint32_t startFrame = 0;

        while (startFrame < context.numFrames)
        {
            auto framesRemaining = context.numFrames - startFrame;
            auto numFramesToDo = framesRemaining < maxBlockSize ? framesRemaining : maxBlockSize;
            prepare (numFramesToDo);

            copyToInterleaved (_getInputFrameArrayRef_audioIn (state).elements, &context.inputChannels[0], startFrame, numFramesToDo);

            advance();

            copyFromInterleaved (&context.outputChannels[0], startFrame, _getOutputFrameArrayRef_audioOut (state).elements, numFramesToDo);
            startFrame += numFramesToDo;
        }
    }

    //==============================================================================
    // The following methods provide low-level access for read/write to all the
    // endpoints directly, and to run the prepare/advance loop.

    void prepare (uint32_t numFramesToBeRendered)
    {
        SOUL_CPP_ASSERT (numFramesToBeRendered != 0);
        framesToAdvance = numFramesToBeRendered;
        _prepare (state, static_cast<int32_t> (numFramesToBeRendered));
    }

    void advance()
    {
        SOUL_CPP_ASSERT (framesToAdvance != 0); // you must call prepare() before advance()!
        auto framesRemaining = framesToAdvance;

        while (framesRemaining > 0)
        {
            auto framesThisCall = framesRemaining < maxBlockSize ? framesRemaining : maxBlockSize;

            run (state, static_cast<int32_t> (framesThisCall));

            totalFramesElapsed += framesThisCall;
            framesRemaining -= framesThisCall;
        }
    }

    void addInputEvent_volume (float eventValue)
    {
        _addInputEvent_volume_f32 (state, eventValue);
    }

    void setNextInputStreamFrames_audioIn (const Vector<float, 2>* frames, uint32_t numFramesToUse)
    {
        auto& buffer = _getInputFrameArrayRef_audioIn (state);

        for (uint32_t i = 0; i < numFramesToUse; ++i)
            buffer[static_cast<int> (i)] = frames[i];
    }

    void setNextInputStreamSparseFrames_audioIn (Vector<float, 2> targetFrameValue, uint32_t numFramesToReachValue, float curveShape)
    {
        _setSparseInputTarget_audioIn (state, targetFrameValue, (int32_t) numFramesToReachValue, curveShape);
    }

    DynamicArray<const Vector<float, 2>> getOutputStreamFrames_audioOut()
    {
        return { &(_getOutputFrameArrayRef_audioOut (state).elements[0]), static_cast<int32_t> (framesToAdvance) };
    }

    //==============================================================================
    // The following methods provide a fixed interface for finding out about
    // the input/output endpoints that this processor provides.

    using EndpointID = const char*;
    enum class EndpointType     { value, stream, event };

    struct EndpointDetails
    {
        const char* name;
        EndpointID endpointID;
        EndpointType endpointType;
        const char* frameType;
        uint32_t numAudioChannels;
        const char* annotation;
    };

    std::vector<EndpointDetails> getInputEndpoints() const
    {
        return
        {
            { "volume",  "in:volume",  EndpointType::event,  "float32",    0, "{ \"label\": \"Volume\", \"min\": -85, \"max\": 6, \"init\": -6, \"step\": 1, \"unit\": \"dB\" }" },
            { "audioIn", "in:audioIn", EndpointType::stream, "float32<2>", 2, ""                                                                                                 }
        };
    }

    std::vector<EndpointDetails> getOutputEndpoints() const
    {
        return
        {
            { "audioOut", "out:audioOut", EndpointType::stream, "float32<2>", 2, "" }
        };
    }

    //==============================================================================
    // The following methods provide help in dealing with the processor's endpoints
    // in a format suitable for traditional audio plugin channels and parameters.

    struct ParameterProperties
    {
        const char* UID;
        const char* name;
        const char* unit;
        float minValue, maxValue, step, initialValue;
        bool isAutomatable, isBoolean, isHidden;
        const char* group;
        const char* textValues;
        float currentValue;
        std::function<void(float)> applyValue;

        void setValue (float f)
        {
            currentValue = snapToLegalValue (f);
            applyValue (f);
        }

        float getValue() const
        {
            return currentValue;
        }

    private:
        float snapToLegalValue (float v) const
        {
            if (step > 0)
                v = minValue + step * std::floor ((v - minValue) / step + 0.5f);

            return v < minValue ? minValue : (v > maxValue ? maxValue : v);
        }
    };

    struct AudioBus
    {
        const char* name;
        uint32_t numChannels;
    };

    static constexpr bool      hasMIDIInput = false;
    static constexpr uint32_t  numParameters = 1;

    std::vector<ParameterProperties> getParameterProperties()
    {
        return
        {
            {  "volume",  "volume",  "dB",  -85.0f,  6.0f,  1.0f,  -6.0f,  true,  false,  false,  "",  "",  -6.0f,  [this] (float v) { addInputEvent_volume (v); }  }
        };
    }

    std::vector<AudioBus> getInputBuses() const   { return { { "audioIn", 2 } }; }

    std::vector<AudioBus> getOutputBuses() const   { return { { "audioOut", 2 } }; }

    //==============================================================================
    struct ZeroInitialiser
    {
        template <typename Type>
        operator Type() const   { return {}; }
    };

    //==============================================================================
    template <typename Type>
    struct DynamicArray
    {
        using ElementType = Type;
        ElementType* elements = nullptr;
        int32_t numElements = 0;

        constexpr ElementType& operator[] (int i) noexcept                   { return elements[i]; }
        constexpr const ElementType& operator[] (int i) const noexcept       { return elements[i]; }
        constexpr int getElementSizeBytes() const noexcept                   { return sizeof (ElementType); }
    };

    //==============================================================================
    template <typename Type, int32_t size>
    struct FixedArray
    {
        using ElementType = Type;
        ElementType elements[size];
        static constexpr int32_t numElements = size;

        static constexpr FixedArray fromRepeatedValue (ElementType value)
        {
            FixedArray a;

            for (auto& element : a.elements)
                element = value;

            return a;
        }

        static size_t elementOffset (int i) noexcept               { return sizeof (ElementType) * i; }
        ElementType& operator[] (int i) noexcept                   { return elements[i]; }
        const ElementType& operator[] (int i) const noexcept       { return elements[i]; }
        int getElementSizeBytes() const noexcept                   { return sizeof (ElementType); }
        DynamicArray<ElementType> toDynamicArray() const noexcept  { return { const_cast<ElementType*> (&elements[0]), size }; }
        operator ElementType*() const noexcept                     { return const_cast<ElementType*> (&elements[0]); }

        FixedArray& operator= (ZeroInitialiser)
        {
            for (auto& e : elements)
                e = {};

            return *this;
        }

        template <int start, int end>
        constexpr FixedArray<Type, end - start> slice() const noexcept
        {
            FixedArray<Type, end - start> newSlice;

            for (int i = 0; i < end - start; ++i)
                newSlice.elements[i] = elements[start + i];

            return newSlice;
        }
    };

    //==============================================================================
    template <typename Type, int32_t size>
    struct Vector
    {
        using ElementType = Type;
        ElementType elements[size] = {};
        static constexpr int32_t numElements = size;

        constexpr Vector() = default;
        constexpr Vector (const Vector&) = default;
        constexpr Vector& operator= (const Vector&) = default;

        explicit constexpr Vector (Type value)
        {
            for (auto& element : elements)
                element = value;
        }

        constexpr Vector (std::initializer_list<Type> i)
        {
            int n = 0;

            for (auto e : i)
                elements[n++] = e;
        }

        static constexpr Vector fromRepeatedValue (Type value)
        {
            return Vector (value);
        }

        constexpr Vector operator+ (const Vector& rhs) const                { return apply<Vector> (rhs, [] (Type a, Type b) { return a + b; }); }
        constexpr Vector operator- (const Vector& rhs) const                { return apply<Vector> (rhs, [] (Type a, Type b) { return a - b; }); }
        constexpr Vector operator* (const Vector& rhs) const                { return apply<Vector> (rhs, [] (Type a, Type b) { return a * b; }); }
        constexpr Vector operator/ (const Vector& rhs) const                { return apply<Vector> (rhs, [] (Type a, Type b) { return a / b; }); }
        constexpr Vector operator% (const Vector& rhs) const                { return apply<Vector> (rhs, [] (Type a, Type b) { return a % b; }); }
        constexpr Vector operator-() const                                  { return apply<Vector> ([] (Type n) { return -n; }); }

        Vector& operator= (ZeroInitialiser)
        {
            for (auto& e : elements)
                e = {};

            return *this;
        }

        constexpr Vector<bool, size> operator== (const Vector& rhs) const   { return apply<Vector<bool, size>> (rhs, [] (Type a, Type b) { return a == b; }); }
        constexpr Vector<bool, size> operator!= (const Vector& rhs) const   { return apply<Vector<bool, size>> (rhs, [] (Type a, Type b) { return a != b; }); }

        template <typename ReturnType, typename Op>
        constexpr ReturnType apply (const Vector& rhs, Op&& op) const noexcept
        {
            ReturnType v;

            for (int i = 0; i < size; ++i)
                v.elements[i] = op (elements[i], rhs.elements[i]);

            return v;
        }

        template <typename ReturnType, typename Op>
        constexpr ReturnType apply (Op&& op) const noexcept
        {
            ReturnType v;

            for (int i = 0; i < size; ++i)
                v.elements[i] = op (elements[i]);

            return v;
        }

        template <int start, int end>
        constexpr Vector<Type, end - start> slice() const noexcept
        {
            Vector<Type, end - start> newSlice;

            for (int i = 0; i < end - start; ++i)
                newSlice.elements[i] = elements[start + i];

            return newSlice;
        }

        constexpr const Type& operator[] (int i) const noexcept  { return elements[i]; }
        constexpr Type& operator[] (int i) noexcept              { return elements[i]; }
    };

    //==============================================================================
    struct StringLiteral
    {
        constexpr StringLiteral (int32_t h) noexcept : handle (h) {}
        StringLiteral() = default;
        StringLiteral (const StringLiteral&) = default;
        StringLiteral& operator= (const StringLiteral&) = default;

        const char* toString() const       { return lookupStringLiteral (handle); }
        operator const char*() const       { return lookupStringLiteral (handle); }

        bool operator== (StringLiteral other) const noexcept    { return handle == other.handle; }
        bool operator!= (StringLiteral other) const noexcept    { return handle != other.handle; }

        int32_t handle = 0;
    };


    //==============================================================================
    //==============================================================================
    //
    //    All the code that follows this point should be considered internal.
    //    User code should rarely need to refer to anything beyond this point..
    //
    //==============================================================================
    //==============================================================================

    template <typename Vec>  static Vec _vec_sqrt  (Vec a)         { return a.template apply<Vec> ([]  (typename Vec::ElementType x) { return std::sqrt (x); }); }
    template <typename Vec>  static Vec _vec_pow   (Vec a, Vec b)  { return a.template apply<Vec> ([&] (typename Vec::ElementType x) { return std::pow (x, b); }); }
    template <typename Vec>  static Vec _vec_exp   (Vec a)         { return a.template apply<Vec> ([]  (typename Vec::ElementType x) { return std::exp (x); }); }
    template <typename Vec>  static Vec _vec_log   (Vec a)         { return a.template apply<Vec> ([]  (typename Vec::ElementType x) { return std::log (x); }); }
    template <typename Vec>  static Vec _vec_log10 (Vec a)         { return a.template apply<Vec> ([]  (typename Vec::ElementType x) { return std::log10 (x); }); }
    template <typename Vec>  static Vec _vec_sin   (Vec a)         { return a.template apply<Vec> ([]  (typename Vec::ElementType x) { return std::sin (x); }); }
    template <typename Vec>  static Vec _vec_cos   (Vec a)         { return a.template apply<Vec> ([]  (typename Vec::ElementType x) { return std::cos (x); }); }
    template <typename Vec>  static Vec _vec_tan   (Vec a)         { return a.template apply<Vec> ([]  (typename Vec::ElementType x) { return std::tan (x); }); }
    template <typename Vec>  static Vec _vec_sinh  (Vec a)         { return a.template apply<Vec> ([]  (typename Vec::ElementType x) { return std::sinh (x); }); }
    template <typename Vec>  static Vec _vec_cosh  (Vec a)         { return a.template apply<Vec> ([]  (typename Vec::ElementType x) { return std::cosh (x); }); }
    template <typename Vec>  static Vec _vec_tanh  (Vec a)         { return a.template apply<Vec> ([]  (typename Vec::ElementType x) { return std::tanh (x); }); }
    template <typename Vec>  static Vec _vec_asinh (Vec a)         { return a.template apply<Vec> ([]  (typename Vec::ElementType x) { return std::asinh (x); }); }
    template <typename Vec>  static Vec _vec_acosh (Vec a)         { return a.template apply<Vec> ([]  (typename Vec::ElementType x) { return std::acosh (x); }); }
    template <typename Vec>  static Vec _vec_atanh (Vec a)         { return a.template apply<Vec> ([]  (typename Vec::ElementType x) { return std::atanh (x); }); }
    template <typename Vec>  static Vec _vec_asin  (Vec a)         { return a.template apply<Vec> ([]  (typename Vec::ElementType x) { return std::asin (x); }); }
    template <typename Vec>  static Vec _vec_acos  (Vec a)         { return a.template apply<Vec> ([]  (typename Vec::ElementType x) { return std::acos (x); }); }
    template <typename Vec>  static Vec _vec_atan  (Vec a)         { return a.template apply<Vec> ([]  (typename Vec::ElementType x) { return std::atan (x); }); }
    template <typename Vec>  static Vec _vec_atan2 (Vec a, Vec b)  { return a.template apply<Vec> ([&] (typename Vec::ElementType x) { return std::atan2 (x, b); }); }

    static constexpr int32_t _intrin_clamp (int32_t n, int32_t low, int32_t high)  { return n < low ? low : (n > high ? high : n); }
    static constexpr int32_t _intrin_wrap (int32_t n, int32_t range)   { if (range == 0) return 0; auto x = n % range; return x < 0 ? x + range : x; }

    static constexpr float  _nan32 = std::numeric_limits<float>::quiet_NaN();
    static constexpr double _nan64 = std::numeric_limits<double>::quiet_NaN();

    static constexpr float  _inf32 = std::numeric_limits<float>::infinity();
    static constexpr double _inf64 = std::numeric_limits<double>::infinity();

    static constexpr float  _ninf32 = -_inf32;
    static constexpr double _ninf64 = -_inf64;

    template <typename SourceFloatType, typename DestFloatType>
    static inline void copyToInterleaved (DestFloatType* monoDest, const SourceFloatType* const* sourceChannels, uint32_t sourceStartFrame, uint32_t numFrames)
    {
        auto source = *sourceChannels + sourceStartFrame;

        for (uint32_t i = 0; i < numFrames; ++i)
            monoDest[i] = static_cast<DestFloatType> (source[i]);
    }

    template <typename SourceFloatType, typename DestFloatType, int numChannels>
    static inline void copyToInterleaved (Vector<DestFloatType, numChannels>* vectorDest, const SourceFloatType* const* sourceChannels, uint32_t sourceStartFrame, uint32_t numFrames)
    {
        for (uint32_t i = 0; i < numFrames; ++i)
            for (uint32_t chan = 0; chan < numChannels; ++chan)
                vectorDest[i].elements[chan] = static_cast<DestFloatType> (sourceChannels[chan][sourceStartFrame + i]);
    }

    template <typename SourceFloatType, typename DestFloatType>
    static inline void copyFromInterleaved (DestFloatType* const* destChannels, uint32_t destStartFrame, const SourceFloatType* monoSource, uint32_t numFrames)
    {
        auto dest = *destChannels + destStartFrame;

        for (uint32_t i = 0; i < numFrames; ++i)
            dest[i] = static_cast<DestFloatType> (monoSource[i]);
    }

    template <typename SourceFloatType, typename DestFloatType, int numChannels>
    static inline void copyFromInterleaved (DestFloatType* const* destChannels, uint32_t destStartFrame, const Vector<SourceFloatType, numChannels>* vectorSource, uint32_t numFrames)
    {
        for (uint32_t i = 0; i < numFrames; ++i)
            for (uint32_t chan = 0; chan < numChannels; ++chan)
                destChannels[chan][destStartFrame + i] = static_cast<DestFloatType> (vectorSource[i].elements[chan]);
    }

    //==============================================================================
    struct _RenderStats
    {
        int32_t m_underrunCount, m_underrunFrames, m_overrunCount, m_overrunFrames;
    };

    struct _SparseStreamStatus
    {
        int32_t m_activeRamps;
        FixedArray<int32_t, 2> m_rampArray;
    };

    struct _Event_in_f32_1
    {
        int32_t m_numFrames;
    };

    struct _Stream_in_vec_2_f32_512
    {
        FixedArray<Vector<float, 2>, 512> m_buffer;
        Vector<float, 2> m_currentSparseValue, m_targetSparseValue, m_perFrameIncrement;
        int32_t m_numSparseFramesToRender, m_constantFilledFrames;
        bool m_sparseStreamActive;
    };

    struct _Stream_out_vec_2_f32_512
    {
        FixedArray<Vector<float, 2>, 512> m_buffer;
    };

    struct soul__gain__SmoothedGainParameter___for__root__Level_smoothedGain___State
    {
        int32_t m__resumePoint, m__frameCount, m__arrayEntry, m__sessionID, m__processorId;
        float m_targetGain, m_currentGain, m_increment;
        int32_t m_remainingRampSamples;
    };

    struct soul__gain__DynamicGain___for__root__Level_gainProcessor___State
    {
        int32_t m__resumePoint, m__frameCount, m__arrayEntry, m__sessionID, m__processorId;
    };

    struct _State
    {
        int32_t m__resumePoint, m__frameCount, m__arrayEntry, m__sessionID, m__processorId, m__framesToAdvance;
        _RenderStats m__renderStats;
        _SparseStreamStatus m__sparseStreamStatus;
        _Event_in_f32_1 m__in_volume;
        _Stream_in_vec_2_f32_512 m__in_audioIn;
        _Stream_out_vec_2_f32_512 m__out_audioOut;
        soul__gain__SmoothedGainParameter___for__root__Level_smoothedGain___State m_smoothedGain_state;
        soul__gain__DynamicGain___for__root__Level_gainProcessor___State m_gainProcessor_state;
    };

    struct soul__gain__SmoothedGainParameter___for__root__Level_smoothedGain___IO
    {
        float m__out_gain;
    };

    struct soul__gain__DynamicGain___for__root__Level_gainProcessor___IO
    {
        Vector<float, 2> m__in_in;
        float m__in_gain;
        Vector<float, 2> m__out_out;
    };

    //==============================================================================
    #if __clang__
     #pragma clang diagnostic push
     #pragma clang diagnostic ignored "-Wunused-label"
     #pragma clang diagnostic ignored "-Wunused-parameter"
     #pragma clang diagnostic ignored "-Wshadow"
    #elif defined(__GNUC__)
     #pragma GCC diagnostic push
     #pragma GCC diagnostic ignored "-Wunused-label"
     #pragma GCC diagnostic ignored "-Wunused-parameter"
     #pragma GCC diagnostic ignored "-Wshadow"
    #elif defined(_MSC_VER)
     #pragma warning (push)
     #pragma warning (disable: 4100 4102 4458)
    #endif

    //==============================================================================
    int32_t run (_State& _state, int32_t maxFrames) noexcept
    {
        int32_t _2 = {};
        Vector<float, 2> _3 = {};
        soul__gain__SmoothedGainParameter___for__root__Level_smoothedGain___IO _4 = {};
        soul__gain__DynamicGain___for__root__Level_gainProcessor___IO _5 = {};

        _2 = _internal___minInt32 (512, maxFrames);
        _updateRampingStreams (_state, _2);
        _state.m__frameCount = 0;
        _main_loop_check: { if (! (_state.m__frameCount < _2)) goto _exit; }
        _main_loop_body: { _3 = _readFromStream_struct__Stream_in_vec_2_f32_512 (_state.m__in_audioIn, _state.m__frameCount);
                           _4 = ZeroInitialiser();
                           soul__gain__SmoothedGainParameter___for__root__Level_smoothedGain__run (_state.m_smoothedGain_state, _4);
                           _5 = ZeroInitialiser();
                           _5.m__in_in = _3;
                           _5.m__in_gain = _4.m__out_gain;
                           soul__gain__DynamicGain___for__root__Level_gainProcessor__run (_state.m_gainProcessor_state, _5);
                           _writeToStream_struct__Stream_out_vec_2_f32_512 (_state.m__out_audioOut, _state.m__frameCount, _5.m__out_out);
                           _state.m__frameCount = _state.m__frameCount + 1;
                           goto _main_loop_check;
        }
        _exit: { _state.m__frameCount = 0;
                 return _2;
        }
    }

    void _initialise (_State& _state, int32_t sessionID) noexcept
    {
        _state.m__sessionID = sessionID;
        _state.m_smoothedGain_state.m__arrayEntry = 0;
        _state.m_smoothedGain_state.m__sessionID = _state.m__sessionID;
        _state.m_smoothedGain_state.m__processorId = 1;
        soul__gain__SmoothedGainParameter___for__root__Level_smoothedGain___initialise (_state.m_smoothedGain_state);
        _state.m_gainProcessor_state.m__arrayEntry = 0;
        _state.m_gainProcessor_state.m__sessionID = _state.m__sessionID;
        _state.m_gainProcessor_state.m__processorId = 2;
    }

    void _addInputEvent_volume_f32 (_State& _state, const float& event) noexcept
    {
        soul__gain__SmoothedGainParameter___for__root__Level_smoothedGain___volume_f32 (_state.m_smoothedGain_state, event);
    }

    FixedArray<Vector<float, 2>, 512>& _getInputFrameArrayRef_audioIn (_State& _state) noexcept
    {
        return _state.m__in_audioIn.m_buffer;
    }

    void _setSparseStream_struct__Stream_in_vec_2_f32_512 (_Stream_in_vec_2_f32_512& streamState, const Vector<float, 2>& targetValue, int32_t framesToReachTarget) noexcept
    {
        float _2 = {};
        Vector<float, 2> rampFrames = {}, delta = {};

        if (! (framesToReachTarget == 0)) goto _ramp;
        _no_ramp: { streamState.m_currentSparseValue = targetValue;
                    streamState.m_targetSparseValue = targetValue;
                    streamState.m_perFrameIncrement = ZeroInitialiser();
                    streamState.m_numSparseFramesToRender = 0;
                    streamState.m_constantFilledFrames = 0;
                    streamState.m_sparseStreamActive = true;
                    return;
        }
        _ramp: { _2 = static_cast<float> (framesToReachTarget);
                 rampFrames = Vector<float, 2> (_2);
                 delta = static_cast<Vector<float, 2>> (targetValue - streamState.m_currentSparseValue);
                 streamState.m_targetSparseValue = targetValue;
                 streamState.m_perFrameIncrement = delta / rampFrames;
                 streamState.m_numSparseFramesToRender = framesToReachTarget;
                 streamState.m_constantFilledFrames = 0;
                 streamState.m_sparseStreamActive = true;
        }
    }

    void _setSparseInputTarget_audioIn (_State& _state, const Vector<float, 2>& targetValue, int32_t framesToReachTarget, float curveShape) noexcept
    {
        if (_state.m__in_audioIn.m_sparseStreamActive) goto _block_1;
        _block_0: { _addRampingStream (_state.m__sparseStreamStatus, 1); }
        _block_1: { _setSparseStream_struct__Stream_in_vec_2_f32_512 (_state.m__in_audioIn, targetValue, framesToReachTarget); }
    }

    FixedArray<Vector<float, 2>, 512>& _getOutputFrameArrayRef_audioOut (_State& state) noexcept
    {
        return state.m__out_audioOut.m_buffer;
    }

    void _prepare (_State& state, int32_t frames) noexcept
    {
        state.m__framesToAdvance = frames;
    }

    int32_t _get_num_xruns (_State& state) noexcept
    {
        return state.m__renderStats.m_underrunCount + state.m__renderStats.m_overrunCount;
    }

    //==============================================================================
    void _renderSparseFrames_struct__Stream_in_vec_2_f32_512 (_Stream_in_vec_2_f32_512& stream, int32_t startFrame, int32_t framesToGenerate) noexcept
    {
        int32_t writePos = {};
        Vector<float, 2> currentValue = {};

        writePos = startFrame;
        currentValue = stream.m_currentSparseValue;
        _main_loop_check: { if (! (framesToGenerate > 0)) goto _exit_after_loop; }
        _main_loop_body: { stream.m_buffer[writePos] = currentValue;
                           currentValue = currentValue + stream.m_perFrameIncrement;
                           writePos = writePos + 1;
                           framesToGenerate = framesToGenerate - 1;
                           goto _main_loop_check;
        }
        _exit_after_loop: { stream.m_currentSparseValue = currentValue; }
    }

    bool _applySparseStreamData_struct__Stream_in_vec_2_f32_512 (_Stream_in_vec_2_f32_512& stream, int32_t numFrames) noexcept
    {
        int32_t rampFrames = {};

        rampFrames = 0;
        if (! (stream.m_sparseStreamActive == true)) goto _exitTrue;
        _check_stream_state: { if (! (stream.m_numSparseFramesToRender == 0)) goto _render_ramp; }
        _no_frames_to_render: { if (stream.m_constantFilledFrames == 512) goto _rampComplete; }
        _add_fixed_value: { stream.m_currentSparseValue = stream.m_targetSparseValue;
                            stream.m_perFrameIncrement = ZeroInitialiser();
                            _renderSparseFrames_struct__Stream_in_vec_2_f32_512 (stream, stream.m_constantFilledFrames, _internal___minInt32 (numFrames, 512 - stream.m_constantFilledFrames));
                            stream.m_constantFilledFrames = stream.m_constantFilledFrames + _internal___minInt32 (numFrames, 512 - stream.m_constantFilledFrames);
                            goto _exit;
        }
        _render_ramp: { rampFrames = _internal___minInt32 (numFrames, stream.m_numSparseFramesToRender);
                        _renderSparseFrames_struct__Stream_in_vec_2_f32_512 (stream, 0, rampFrames);
                        stream.m_numSparseFramesToRender = stream.m_numSparseFramesToRender - rampFrames;
                        if (rampFrames == numFrames) goto _exit;
        }
        _fill_remainder: { stream.m_currentSparseValue = stream.m_targetSparseValue;
                           stream.m_perFrameIncrement = ZeroInitialiser();
                           _renderSparseFrames_struct__Stream_in_vec_2_f32_512 (stream, rampFrames, numFrames - rampFrames);
        }
        _exit: { return false; }
        _exitTrue: { return true; }
        _rampComplete: { stream.m_sparseStreamActive = false;
                         return true;
        }
    }

    void _addRampingStream (_SparseStreamStatus& status, int32_t streamId) noexcept
    {
        status.m_rampArray[status.m_activeRamps] = streamId;
        status.m_activeRamps = status.m_activeRamps + 1;
    }

    bool _updateRampingStream (_State& _state, int32_t streamId, int32_t framesToRender) noexcept
    {
        bool rampComplete = {};

        rampComplete = false;
        if (! (streamId == 1)) goto _exit;
        _case_1: { rampComplete = _applySparseStreamData_struct__Stream_in_vec_2_f32_512 (_state.m__in_audioIn, framesToRender); }
        _exit: { return rampComplete; }
    }

    void _updateRampingStreams (_State& _state, int32_t framesToRender) noexcept
    {
        bool rampComplete = {};
        int32_t readPos = {}, writePos = {};

        rampComplete = false;
        readPos = 0;
        writePos = 0;
        if (_state.m__sparseStreamStatus.m_activeRamps == 0) goto _exit;
        _loop: { rampComplete = _updateRampingStream (_state, _state.m__sparseStreamStatus.m_rampArray[readPos], framesToRender);
                 if (rampComplete) goto _rampComplete;
        }
        _rampActive: { _state.m__sparseStreamStatus.m_rampArray[writePos] = _state.m__sparseStreamStatus.m_rampArray[readPos];
                       readPos = readPos + 1;
                       writePos = writePos + 1;
                       if (readPos == _state.m__sparseStreamStatus.m_activeRamps) goto _loopExit;
                       goto _loop;
        }
        _rampComplete: { readPos = readPos + 1;
                         if (! (readPos == _state.m__sparseStreamStatus.m_activeRamps)) goto _loop;
        }
        _loopExit: { _state.m__sparseStreamStatus.m_activeRamps = writePos; }
        _exit: {}
    }

    Vector<float, 2> _readFromStream_struct__Stream_in_vec_2_f32_512 (const _Stream_in_vec_2_f32_512& stream, int32_t readPos) noexcept
    {
        Vector<float, 2> _2 = {};

        _2 = stream.m_buffer[readPos];
        return _2;
    }

    void _writeToStream_struct__Stream_out_vec_2_f32_512 (_Stream_out_vec_2_f32_512& stream, int32_t writePos, Vector<float, 2> value) noexcept
    {
        stream.m_buffer[writePos] = value;
    }

    //==============================================================================
    float soul__dBtoGain (float decibels) noexcept
    {
        float _2 = {}, _3 = {}, _4 = {}, _T0 = {};

        if (! (decibels > -100.0f)) goto _ternary_false_0;
        _ternary_true_0: { _2 = std::pow (10.0f, decibels * 0.05f);
                           _T0 = _2;
                           goto _ternary_end_0;
        }
        _ternary_false_0: { _3 = 0;
                            _T0 = _3;
        }
        _ternary_end_0: { _4 = _T0;
                          return _4;
        }
    }

    //==============================================================================
    float soul__intrinsics___pow_specialised_2_f32_f32 (float a, float b) noexcept
    {
        return 0;
    }

    float soul__intrinsics___abs_specialised_1_f32 (float n) noexcept
    {
        float _2 = {}, _3 = {}, _4 = {}, _T5 = {};

        if (! (n < 0)) goto _ternary_false_5;
        _ternary_true_5: { _2 = -n;
                           _T5 = _2;
                           goto _ternary_end_5;
        }
        _ternary_false_5: { _3 = n;
                            _T5 = _3;
        }
        _ternary_end_5: { _4 = _T5;
                          return _4;
        }
    }

    int32_t soul__intrinsics___max_specialised_2_i32_i32 (int32_t a, int32_t b) noexcept
    {
        int32_t _2 = {}, _3 = {}, _4 = {}, _T6 = {};

        if (! (a > b)) goto _ternary_false_6;
        _ternary_true_6: { _2 = a;
                           _T6 = _2;
                           goto _ternary_end_6;
        }
        _ternary_false_6: { _3 = b;
                            _T6 = _3;
        }
        _ternary_end_6: { _4 = _T6;
                          return _4;
        }
    }

    //==============================================================================
    void soul__gain__SmoothedGainParameter___for__root__Level_smoothedGain___volume_f32 (soul__gain__SmoothedGainParameter___for__root__Level_smoothedGain___State& _state, float targetDB) noexcept
    {
        float _2 = {}, _3 = {};
        float maxDelta = {};
        int32_t _4 = {};

        _2 = soul__dBtoGain (targetDB);
        _state.m_targetGain = _2;
        maxDelta = static_cast<float> (static_cast<float> ((1.0 / (sampleRate * 1.0)) / 0.009999999776482582));
        _3 = soul__intrinsics___abs_specialised_1_f32 (_state.m_targetGain - _state.m_currentGain);
        _4 = soul__intrinsics___max_specialised_2_i32_i32 (1, static_cast<int32_t> (_3 / static_cast<float> (maxDelta)));
        _state.m_remainingRampSamples = _4;
        _state.m_increment = (_state.m_targetGain - _state.m_currentGain) / static_cast<float> (_state.m_remainingRampSamples);
    }

    void soul__gain__SmoothedGainParameter___for__root__Level_smoothedGain__run (soul__gain__SmoothedGainParameter___for__root__Level_smoothedGain___State& _state, soul__gain__SmoothedGainParameter___for__root__Level_smoothedGain___IO& _io) noexcept
    {
        float out_value_gain = {};
        int32_t _2 = {}, _3 = {};

        out_value_gain = 0;
        if (! (_state.m_remainingRampSamples != 0)) goto _ifnot_0;
        _if_0: { _2 = _state.m_remainingRampSamples;
                 _3 = _2 - 1;
                 _state.m_remainingRampSamples = _3;
                 if (! (_3 == 0)) goto _ifnot_1;
        }
        _if_1: { _state.m_currentGain = _state.m_targetGain;
                 goto _ifnot_0;
        }
        _ifnot_1: { _state.m_currentGain = _state.m_currentGain + _state.m_increment; }
        _ifnot_0: { out_value_gain = out_value_gain + _state.m_currentGain;
                    _state.m__resumePoint = 1;
                    _io.m__out_gain = out_value_gain;
        }
    }

    void soul__gain__SmoothedGainParameter___for__root__Level_smoothedGain___initialise (soul__gain__SmoothedGainParameter___for__root__Level_smoothedGain___State& _state) noexcept
    {
        _state.m_targetGain = 0;
        _state.m_currentGain = 0;
        _state.m_increment = 0;
        _state.m_remainingRampSamples = 0;
    }

    //==============================================================================
    void soul__gain__DynamicGain___for__root__Level_gainProcessor__run (soul__gain__DynamicGain___for__root__Level_gainProcessor___State& _state, soul__gain__DynamicGain___for__root__Level_gainProcessor___IO& _io) noexcept
    {
        Vector<float, 2> out_value_out = {}, _2 = {};
        float _3 = {};

        out_value_out = ZeroInitialiser();
        _2 = _io.m__in_in;
        _3 = _io.m__in_gain;
        out_value_out = out_value_out + (_2 * Vector<float, 2> (_3));
        _state.m__resumePoint = 1;
        _io.m__out_out = out_value_out;
    }

    //==============================================================================
    int32_t _internal___minInt32 (int32_t a, int32_t b) noexcept
    {
        if (! (a < b)) goto _moreThan;
        _lessThan: { return a; }
        _moreThan: { return b; }
    }


    #if __clang__
     #pragma clang diagnostic pop
    #elif defined(__GNUC__)
     #pragma GCC diagnostic pop
    #elif defined(_MSC_VER)
     #pragma warning (pop)
    #endif

    //==============================================================================
    // The program contains no string literals, so this function should never be called
    static constexpr const char* lookupStringLiteral (int32_t)  { return {}; }

    //==============================================================================
    _State state = {}, initialisedState;

    double sampleRate = 1.0;
    uint32_t framesToAdvance = 0;
    uint64_t totalFramesElapsed = 0;
};

//==============================================================================
//==============================================================================
//
// The rest of this file is now the implementation of juce::AudioPluginInstance
//
//==============================================================================
//==============================================================================

struct Level::Pimpl
{
    Pimpl (Level& p) : owner (p) {}

    using GeneratedClass = SOUL_Level;

    Level& owner;
    GeneratedClass processor;
    int sessionID = 0;
    std::atomic<uint32_t> numParametersNeedingUpdate { 0 };

    juce::AudioBuffer<float> outputBuffer;
    std::vector<GeneratedClass::MIDIMessage> incomingMIDIMessages;

    struct IDs
    {
        const juce::Identifier UID      { GeneratedClass::UID },
                               id       { "id" },
                               version  { "version" },
                               value    { "value" },
                               size     { "size" },
                               PARAM    { "PARAM" },
                               EDITOR   { "EDITOR" };
    };

    IDs ids;

    static juce::AudioProcessor::BusesProperties createBuses()
    {
        juce::AudioProcessor::BusesProperties buses;

        if constexpr (GeneratedClass::numAudioInputChannels > 0)
            buses.addBus (true, "Input", juce::AudioChannelSet::canonicalChannelSet (GeneratedClass::numAudioInputChannels));

        if constexpr (GeneratedClass::numAudioOutputChannels > 0)
            buses.addBus (false, "Output", juce::AudioChannelSet::canonicalChannelSet (GeneratedClass::numAudioOutputChannels));

        return buses;
    }

    void prepareToPlay (double sampleRate, int maxBlockSize)
    {
        processor.init (sampleRate, ++sessionID);

        incomingMIDIMessages.resize ((size_t) maxBlockSize);
        owner.setRateAndBufferSizeDetails (sampleRate, maxBlockSize);
        owner.midiKeyboardState.reset();
        outputBuffer.setSize (GeneratedClass::numAudioOutputChannels, maxBlockSize, false, false, true);
        updateAllParameters();
    }

    template <class RenderContext>
    static void populateInputChannels (juce::AudioBuffer<float>& audio, RenderContext& rc)
    {
        if constexpr (GeneratedClass::numAudioInputChannels > 0)
        {
            for (int i = 0; i < (int) GeneratedClass::numAudioInputChannels; ++i)
                rc.inputChannels[i] = audio.getReadPointer (i);
        }
    }

    void processBlock (juce::AudioBuffer<float>& audio, juce::MidiBuffer& midi)
    {
        auto numFrames = audio.getNumSamples();
        outputBuffer.setSize (GeneratedClass::numAudioOutputChannels, numFrames, false, false, true);
        outputBuffer.clear();

        GeneratedClass::RenderContext<float> rc;

        populateInputChannels (audio, rc);

        for (int i = 0; i < (int) GeneratedClass::numAudioOutputChannels; ++i)
            rc.outputChannels[i] = outputBuffer.getWritePointer (i);

        rc.numFrames = (uint32_t) numFrames;

        owner.midiKeyboardState.processNextMidiBuffer (midi, 0, numFrames, true);

        if (midi.isEmpty())
        {
            rc.incomingMIDI.numMessages = 0;
        }
        else
        {
            auto maxEvents = incomingMIDIMessages.size();
            auto iter = midi.cbegin();
            auto end = midi.cend();
            size_t i = 0;

            while (i < maxEvents && iter != end)
            {
                auto message = *iter++;

                if (message.numBytes < 4)
                    incomingMIDIMessages[i++] = { static_cast<uint32_t> (message.samplePosition),
                                                  static_cast<uint8_t> (message.data[0]),
                                                  static_cast<uint8_t> (message.data[1]),
                                                  static_cast<uint8_t> (message.data[2]) };
            }

            rc.incomingMIDI.messages = std::addressof (incomingMIDIMessages[0]);
            rc.incomingMIDI.numMessages = (uint32_t) i;
            midi.clear();
        }

        updateAnyChangedParameters();

        processor.render (rc);

        for (int i = 0; i < outputBuffer.getNumChannels(); ++i)
            audio.copyFrom (i, 0, outputBuffer, i, 0, numFrames);
    }

    void updateAllParameters();
    void updateAnyChangedParameters();
};

//==============================================================================
Level::Level()
   : juce::AudioPluginInstance (Pimpl::createBuses())
{
    pimpl = std::make_unique<Pimpl> (*this);
    refreshParameterList();
}

Level::~Level() = default;

//==============================================================================
void Level::fillInPluginDescription (juce::PluginDescription& d) const
{
    d.name                = Pimpl::GeneratedClass::name;
    d.descriptiveName     = Pimpl::GeneratedClass::description;
    d.pluginFormatName    = "Compiled SOUL Patch";
    d.category            = Pimpl::GeneratedClass::category;
    d.manufacturerName    = Pimpl::GeneratedClass::manufacturer;
    d.version             = Pimpl::GeneratedClass::version;
    d.fileOrIdentifier    = {};
    d.lastFileModTime     = {};
    d.lastInfoUpdateTime  = {};
    d.uid                 = (int) juce::String (Pimpl::GeneratedClass::UID).hash();
    d.isInstrument        = Pimpl::GeneratedClass::isInstrument;
}

const juce::String Level::getName() const    { return Pimpl::GeneratedClass::name; }

juce::StringArray Level::getAlternateDisplayNames() const
{
    juce::StringArray s;
    s.add (Pimpl::GeneratedClass::name);

    if (Pimpl::GeneratedClass::description[0] != 0)
        s.add (Pimpl::GeneratedClass::description);

    return s;
}

bool Level::isBusesLayoutSupported (const BusesLayout& layout) const
{
    auto processorInputBuses  = pimpl->processor.getInputBuses();
    auto processorOutputBuses = pimpl->processor.getOutputBuses();

    if (layout.inputBuses.size() != (int) processorInputBuses.size())
        return false;

    if (layout.outputBuses.size() != (int) processorOutputBuses.size())
        return false;

    for (size_t i = 0; i < processorInputBuses.size(); ++i)
        if ((int) processorInputBuses[i].numChannels != layout.getNumChannels (true, (int) i))
            return false;

    for (size_t i = 0; i < processorOutputBuses.size(); ++i)
        if ((int) processorOutputBuses[i].numChannels != layout.getNumChannels (false, (int) i))
            return false;

    return true;
}

//==============================================================================
void Level::reset()                                                { pimpl->processor.reset(); }
void Level::prepareToPlay (double sampleRate, int maxBlockSize)    { pimpl->prepareToPlay (sampleRate, maxBlockSize); }
void Level::releaseResources()                                     { midiKeyboardState.reset(); }

void Level::processBlock (juce::AudioBuffer<float>& audio, juce::MidiBuffer& midi)
{
    if (isSuspended())
    {
        for (int i = 0; i < (int) Pimpl::GeneratedClass::numAudioOutputChannels; ++i)
            audio.clear (i, 0, audio.getNumSamples());
    }
    else
    {
        pimpl->processBlock (audio, midi);
    }
}

//==============================================================================
int Level::getNumPrograms()                               { return 1; }
int Level::getCurrentProgram()                            { return 0; }
void Level::setCurrentProgram (int)                       {}
const juce::String Level::getProgramName (int)            { return {}; }
void Level::changeProgramName (int, const juce::String&)  {}

double Level::getTailLengthSeconds() const                { return 0; }
bool Level::acceptsMidi() const                           { return Pimpl::GeneratedClass::hasMIDIInput; }
bool Level::producesMidi() const                          { return false; }
bool Level::supportsMPE() const                           { return Pimpl::GeneratedClass::hasMIDIInput; }
bool Level::isMidiEffect() const                          { return acceptsMidi() && producesMidi(); }
void Level::setNonRealtime (bool) noexcept                {}

//==============================================================================
struct Level::Parameter  : public juce::AudioProcessorParameterWithID
{
    Parameter (Level& owner, const Pimpl::GeneratedClass::ParameterProperties& p)
        : AudioProcessorParameterWithID (p.UID, p.name),
          properties (p),
          textValues (parseTextValues (properties.textValues)),
          range (properties.minValue, properties.maxValue, properties.step),
          numDecimalPlaces (getNumDecimalPlaces (range)),
          numParametersNeedingUpdate (owner.pimpl->numParametersNeedingUpdate)
    {
        currentFullRangeValue = properties.initialValue;
    }

    float currentFullRangeValue = 0;
    bool needsUpdate = false;

    Pimpl::GeneratedClass::ParameterProperties properties;
    const juce::StringArray textValues;
    const juce::NormalisableRange<float> range;
    const int numDecimalPlaces;
    std::atomic<uint32_t>& numParametersNeedingUpdate;

    juce::String getName (int maximumStringLength) const override    { return name.substring (0, maximumStringLength); }
    juce::String getLabel() const override                           { return properties.unit; }
    Category getCategory() const override                            { return genericParameter; }
    bool isDiscrete() const override                                 { return range.interval != 0; }
    bool isBoolean() const override                                  { return properties.isBoolean; }
    bool isAutomatable() const override                              { return properties.isAutomatable; }
    bool isMetaParameter() const override                            { return false; }
    juce::StringArray getAllValueStrings() const override            { return textValues; }
    float getDefaultValue() const override                           { return convertTo0to1 (properties.initialValue); }
    float getValue() const override                                  { return convertTo0to1 (currentFullRangeValue); }
    void setValue (float newValue) override                          { setFullRangeValue (convertFrom0to1 (newValue)); }

    void setFullRangeValue (float newValue)
    {
        if (newValue != currentFullRangeValue)
        {
            currentFullRangeValue = newValue;

            if (! needsUpdate)
            {
                needsUpdate = true;
                ++numParametersNeedingUpdate;
            }

            sendValueChangedMessageToListeners (convertTo0to1 (newValue));
        }
    }

    void sendUpdate()
    {
        properties.setValue (currentFullRangeValue);
    }

    bool sendUpdateIfNeeded()
    {
        if (! needsUpdate)
            return false;

        needsUpdate = false;
        sendUpdate();
        return true;
    }

    juce::String getText (float v, int length) const override
    {
        if (length > 0)              return getText (v, 0).substring (0, length);
        if (textValues.isEmpty())    return juce::String (convertFrom0to1 (v), numDecimalPlaces);
        if (textValues.size() == 1)  return preprocessText (textValues[0].toUTF8(), convertFrom0to1 (v));

        return textValues[juce::jlimit (0, textValues.size() - 1, juce::roundToInt (v * (textValues.size() - 1.0f)))];
    }

    float getValueForText (const juce::String& text) const override
    {
        for (int i = 0; i < textValues.size(); ++i)
            if (textValues[i] == text)
                return i / (textValues.size() - 1.0f);

        return convertTo0to1 (text.upToLastOccurrenceOf (text, false, false).getFloatValue());
    }

    int getNumSteps() const override
    {
        if (! textValues.isEmpty() && std::abs (textValues.size() - (range.end - range.start)) < 0.01f)
            return textValues.size() - 1;

        if (range.interval > 0)
            return static_cast<int> ((range.end - range.start) / range.interval) + 1;

        return AudioProcessor::getDefaultNumParameterSteps();
    }

private:
    float convertTo0to1 (float v) const    { return range.convertTo0to1 (range.snapToLegalValue (v)); }
    float convertFrom0to1 (float v) const  { return range.snapToLegalValue (range.convertFrom0to1 (juce::jlimit (0.0f, 1.0f, v))); }

    static int getNumDecimalPlaces (juce::NormalisableRange<float> r)
    {
        int places = 7;

        if (r.interval != 0.0f)
        {
            if (juce::approximatelyEqual (std::abs (r.interval - (int) r.interval), 0.0f))
                return 0;

            auto v = std::abs (juce::roundToInt (r.interval * pow (10, places)));

            while ((v % 10) == 0 && places > 0)
            {
                --places;
                v /= 10;
            }
        }

        return places;
    }

    static juce::StringArray parseTextValues (const juce::String& text)
    {
        if (text.isNotEmpty())
            return juce::StringArray::fromTokens (text.unquoted(), "|", {});

        return {};
    }

    static juce::String preprocessText (juce::CharPointer_UTF8 text, float value)
    {
        juce::MemoryOutputStream result;

        while (! text.isEmpty())
        {
            auto c = text.getAndAdvance();

            if (c != '%')  { result << juce::String::charToString (c); continue; }

            auto format = text;
            bool addSignChar = (*format == '+');
            if (addSignChar) ++format;

            bool isPadded = (*format == '0');
            int numDigits = 0;

            while (format.isDigit())
                numDigits = numDigits * 10 + (format.getAndAdvance() - '0');

            bool isFloat = (*format == 'f');
            bool isInt   = (*format == 'd');

            if (! (isInt || isFloat))
            {
                result << '%';
                continue;
            }

            if (addSignChar && value >= 0)
                result << '+';

            if (isInt)
            {
                juce::String s ((int64_t) (value + 0.5f));
                result << (isPadded ? s.paddedLeft ('0', numDigits) : s);
            }
            else if (numDigits <= 0)
            {
                result << value;
            }
            else if (isPadded)
            {
                result << juce::String (value, numDigits);
            }
            else
            {
                juce::String s (value);
                auto afterDot = s.fromLastOccurrenceOf (".", false, false);

                if (afterDot.containsOnly ("0123456789"))
                    if (afterDot.length() > numDigits)
                        s = s.dropLastCharacters (afterDot.length() - numDigits);

                result << s;
            }

            text = ++format;
        }

        return result.toString();
    }
};

void Level::Pimpl::updateAllParameters()
{
    for (auto& p : owner.allParameters)
        p->sendUpdate();
}

void Level::Pimpl::updateAnyChangedParameters()
{
    if (numParametersNeedingUpdate != 0)
        for (auto& p : owner.allParameters)
            if (p->sendUpdateIfNeeded())
                if (--numParametersNeedingUpdate == 0)
                    break;
}

void Level::refreshParameterList()
{
    struct ParameterTreeGroupBuilder
    {
        std::map<juce::String, juce::AudioProcessorParameterGroup*> groups;
        juce::AudioProcessorParameterGroup tree;

        void addParam (std::unique_ptr<Parameter> newParam)
        {
            juce::String group (newParam->properties.group);

            if (group.isNotEmpty())
                getOrCreateGroup (tree, {}, group).addChild (std::move (newParam));
            else
                tree.addChild (std::move (newParam));
        }

        juce::AudioProcessorParameterGroup& getOrCreateGroup (juce::AudioProcessorParameterGroup& targetTree,
                                                              const juce::String& parentPath,
                                                              const juce::String& subPath)
        {
            auto fullPath = parentPath + "/" + subPath;
            auto& targetGroup = groups[fullPath];

            if (targetGroup != nullptr)
                return *targetGroup;

            auto slash = subPath.indexOfChar ('/');

            if (slash < 0)
            {
                auto newGroup = std::make_unique<juce::AudioProcessorParameterGroup> (fullPath, subPath, "/");
                targetGroup = newGroup.get();
                targetTree.addChild (std::move (newGroup));
                return *targetGroup;
            }

            auto firstPathPart = subPath.substring (0, slash);
            auto& parentGroup = getOrCreateGroup (targetTree, parentPath, firstPathPart);
            return getOrCreateGroup (parentGroup, parentPath + "/" + firstPathPart, subPath.substring (slash + 1));
        }
    };

    ParameterTreeGroupBuilder treeBuilder;

    for (auto& p : pimpl->processor.getParameterProperties())
    {
        auto param = std::make_unique<Parameter> (*this, p);
        allParameters.push_back (param.get());

        if (p.isHidden)
            hiddenParams.push_back (std::move (param));
        else
            treeBuilder.addParam (std::move (param));
    }

    setParameterTree (std::move (treeBuilder.tree));
    pimpl->numParametersNeedingUpdate = static_cast<uint32_t> (allParameters.size());
}

//==============================================================================
void Level::getStateInformation (juce::MemoryBlock& data)
{
    updateLastState();
    juce::MemoryOutputStream out (data, false);
    lastValidState.writeToStream (out);
}

void Level::setStateInformation (const void* data, int size)
{
    auto newState = juce::ValueTree::readFromData (data, (size_t) size);

    if (newState.hasType (pimpl->ids.UID))
    {
        lastValidState = std::move (newState);
        applyLastState();
    }
}

juce::ValueTree Level::createCurrentState()
{
    juce::ValueTree state (pimpl->ids.UID);
    state.setProperty (pimpl->ids.version, Pimpl::GeneratedClass::version, nullptr);

    auto editorState = lastValidState.getChildWithName (pimpl->ids.EDITOR);

    if (editorState.isValid())
        state.addChild (editorState.createCopy(), -1, nullptr);

    for (auto& p : allParameters)
    {
        juce::ValueTree param (pimpl->ids.PARAM);
        param.setProperty (pimpl->ids.id, p->properties.UID, nullptr);
        param.setProperty (pimpl->ids.value, p->currentFullRangeValue, nullptr);
        state.addChild (param, -1, nullptr);
    }

    return state;
}

void Level::updateLastState()
{
    lastValidState = createCurrentState();
}

void Level::ensureValidStateExists()
{
    if (! lastValidState.hasType (pimpl->ids.UID))
        updateLastState();
}

void Level::applyLastState()
{
    if (lastValidState.hasType (pimpl->ids.UID))
        for (auto& p : allParameters)
            if (auto* value = lastValidState.getChildWithProperty (pimpl->ids.id, p->properties.UID).getPropertyPointer (pimpl->ids.value))
                p->setFullRangeValue (*value);
}

//==============================================================================
struct Level::EditorComponent  : public juce::AudioProcessorEditor
{
    EditorComponent (Level& p)
        : juce::AudioProcessorEditor (p), owner (p), editor (p),
          midiKeyboard (p.midiKeyboardState, juce::MidiKeyboardComponent::Orientation::horizontalKeyboard)
    {
        setLookAndFeel (&lookAndFeel);

        if constexpr (Pimpl::GeneratedClass::numParameters != 0)
            addAndMakeVisible (editor);

        if (Pimpl::GeneratedClass::hasMIDIInput)
            addAndMakeVisible (midiKeyboard);

        auto size = owner.getStoredEditorSize ({ 600, 400 });
        setResizeLimits (400, 150, 2000, 2000);
        setSize (size.width, size.height);
    }

    ~EditorComponent() override
    {
        owner.editorBeingDeleted (this);
        setLookAndFeel (nullptr);
    }

    void paint (juce::Graphics& g) override
    {
        auto background = lookAndFeel.findColour (juce::ResizableWindow::backgroundColourId);
        g.fillAll (background);

        if (getNumChildComponents() == 0)
        {
            g.setColour (background.contrasting());
            g.setFont (16.0f);
            g.drawFittedText (owner.getName(), getLocalBounds().reduced (6), juce::Justification::centred, 2);
        }
    }

    void resized() override
    {
        auto r = getLocalBounds().reduced (6);

        if (midiKeyboard.isVisible())
            midiKeyboard.setBounds (r.removeFromBottom (std::min (80, r.getHeight() / 4)));

        editor.setBounds (r);
        owner.storeEditorSize ({ getWidth(), getHeight() });
    }

    Level& owner;
    juce::LookAndFeel_V4 lookAndFeel;
    juce::GenericAudioProcessorEditor editor;
    juce::MidiKeyboardComponent midiKeyboard;
};

bool Level::hasEditor() const                         { return true; }
juce::AudioProcessorEditor* Level::createEditor()     { return new EditorComponent (*this); }

Level::EditorSize Level::getStoredEditorSize (EditorSize defaultSize)
{
    auto propertyValue = lastValidState.getChildWithName (pimpl->ids.EDITOR).getProperty (pimpl->ids.size);
    auto tokens = juce::StringArray::fromTokens (propertyValue.toString(), " ", {});

    if (tokens.size() == 2)
    {
        auto w = tokens[0].getIntValue();
        auto h = tokens[1].getIntValue();

        if (w > 0 && h > 0)
            return { w, h };
    }

    return defaultSize;
}

void Level::storeEditorSize (EditorSize newSize)
{
    ensureValidStateExists();
    auto state = lastValidState.getOrCreateChildWithName (pimpl->ids.EDITOR, nullptr);

    if (newSize.width > 0 || newSize.height > 0)
        state.setProperty (pimpl->ids.size, juce::String (newSize.width) + " " + juce::String (newSize.height), nullptr);
    else
        state.removeProperty (pimpl->ids.size, nullptr);
}

