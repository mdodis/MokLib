#pragma once
#include "Base.h"
#include "Delegates.h"
#include "Containers/Array.h"

/**
 * A Finite state machine template
 */

struct FSMState {
    Delegate<void> on_enter;
    FSMState() = default;
    FORWARD_DELEGATE_RAW(constexpr FSMState, Delegate<void>, on_enter, )
};

struct FSMTransition {
    u32 from_state;
    u32 to_state;
    typedef Delegate<void, float> UpdateDelegate;
    UpdateDelegate on_update;

    FSMTransition()
        : from_state(U32::Max)
        , to_state(U32::Max)
        {}

    FORWARD_DELEGATE_TEMPLATE()
    constexpr FSMTransition(
        u32 from, u32 to,
        FORWARD_DELEGATE_RAW_SIG(UpdateDelegate))
    {
        from_state = from;
        to_state = to;
        FORWARD_DELEGATE_RAW_BOD(on_update);
    }

};

template <u32 NumStates, u32 NumTransitions>
struct TFSM {
    TArr<FSMState, NumStates> states;
    TArr<FSMTransition, NumTransitions> transitions;

    float current_time = 0.f;
    u32 current_state = 0, target_state = 0;
    FSMTransition *current_transition;

    void change_to(u32 state) {
        target_state = state;

        current_transition = 0;
        // for (FSMTransition &transition : transitions) {
        //     if (transition.from_state == current_state) {
        //         current_transition = &transition;
        //         break;
        //     }
        // }

        if (!current_transition) {
            current_state = target_state;
            states[current_state].on_enter.call();
        }
    }
};