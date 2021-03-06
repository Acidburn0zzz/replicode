//	guard_builder.h
//
//	Author: Eric Nivel
//
//	BSD license:
//	Copyright (c) 2010, Eric Nivel
//	All rights reserved.
//	Redistribution and use in source and binary forms, with or without
//	modification, are permitted provided that the following conditions are met:
//
//   - Redistributions of source code must retain the above copyright
//     notice, this list of conditions and the following disclaimer.
//   - Redistributions in binary form must reproduce the above copyright
//     notice, this list of conditions and the following disclaimer in the
//     documentation and/or other materials provided with the distribution.
//   - Neither the name of Eric Nivel nor the
//     names of their contributors may be used to endorse or promote products
//     derived from this software without specific prior written permission.
//
//	THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND ANY
//	EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
//	WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
//	DISCLAIMED. IN NO EVENT SHALL THE REGENTS AND CONTRIBUTORS BE LIABLE FOR ANY
//	DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
//	(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
//	LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
//	ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
//	(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
//	SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#ifndef guard_builder_h
#define guard_builder_h

#include "factory.h"


namespace r_exec {

class GuardBuilder:
    public _Object {
public:
    GuardBuilder();
    virtual ~GuardBuilder();

    virtual void build(Code *mdl, _Fact *premise_pattern, _Fact *cause_pattern, uint16_t &write_index) const;
};

// fwd: t2=t0+period, t3=t1+period.
// bwd: t0=t2-period, t1=t3-period.
class TimingGuardBuilder:
    public GuardBuilder {
protected:
    uint64_t period;

    void write_guard(Code *mdl, uint16_t l, uint16_t r, uint16_t opcode, uint64_t offset, uint16_t &write_index, uint16_t &extent_index) const;
    void _build(Code *mdl, uint16_t t0, uint16_t t1, uint16_t &write_index) const;
public:
    TimingGuardBuilder(uint64_t period);
    virtual ~TimingGuardBuilder();

    virtual void build(Code *mdl, _Fact *premise_pattern, _Fact *cause_pattern, uint16_t &write_index) const;
};

// fwd: q1=q0+speed*period.
// bwd: speed=(q1-q0)/period, speed.after=q1.after-offset, speed.before=q1.before-offset.
class SGuardBuilder:
    public TimingGuardBuilder {
private:
    uint64_t offset; // period-(speed.after-t0).

    void _build(Code *mdl, uint16_t q0, uint16_t t0, uint16_t t1, uint16_t &write_index) const;
public:
    SGuardBuilder(uint64_t period, uint64_t offset);
    ~SGuardBuilder();

    void build(Code *mdl, _Fact *premise_pattern, _Fact *cause_pattern, uint16_t &write_index) const;
};

// bwd: cmd.after=q1.after-offset, cmd.before=cmd.after+cmd_duration.
class NoArgCmdGuardBuilder:
    public TimingGuardBuilder {
protected:
    uint64_t offset;
    uint64_t cmd_duration;

    void _build(Code *mdl, uint16_t q0, uint16_t t0, uint16_t t1, uint16_t &write_index) const;
public:
    NoArgCmdGuardBuilder(uint64_t period, uint64_t offset, uint64_t cmd_duration);
    ~NoArgCmdGuardBuilder();

    void build(Code *mdl, _Fact *premise_pattern, _Fact *cause_pattern, uint16_t &write_index) const;
};

// bwd: cmd.after=q1.after-period, cmd.before=q1.before-period.
class CmdGuardBuilder:
    public TimingGuardBuilder {
protected:
    uint16_t cmd_arg_index;

    void _build(Code *mdl, uint16_t fwd_opcode, uint16_t bwd_opcode, uint16_t q0, uint16_t t0, uint16_t t1, uint16_t &write_index) const;
    void _build(Code *mdl, uint16_t fwd_opcode, uint16_t bwd_opcode, _Fact *premise_pattern, _Fact *cause_pattern, uint16_t &write_index) const;

    CmdGuardBuilder(uint64_t period, uint16_t cmd_arg_index);
public:
    virtual ~CmdGuardBuilder();
};

// fwd: q1=q0*cmd_arg.
// bwd: cmd_arg=q1/q0.
class MCGuardBuilder:
    public CmdGuardBuilder {
public:
    MCGuardBuilder(uint64_t period, double cmd_arg_index);
    ~MCGuardBuilder();

    void build(Code *mdl, _Fact *premise_pattern, _Fact *cause_pattern, uint16_t &write_index) const;
};

// fwd: q1=q0+cmd_arg.
// bwd: cmd_arg=q1-q0.
class ACGuardBuilder:
    public CmdGuardBuilder {
private:

public:
    ACGuardBuilder(uint64_t period, uint16_t cmd_arg_index);
    ~ACGuardBuilder();

    void build(Code *mdl, _Fact *premise_pattern, _Fact *cause_pattern, uint16_t &write_index) const;
};

// bwd: cause.after=t2-offset, cause.before=t3-offset.
class ConstGuardBuilder:
    public TimingGuardBuilder {
protected:
    double constant;
    uint64_t offset;

    void _build(Code *mdl, uint16_t fwd_opcode, uint16_t bwd_opcode, uint16_t q0, uint16_t t0, uint16_t t1, uint16_t &write_index) const;
    void _build(Code *mdl, uint16_t fwd_opcode, uint16_t bwd_opcode, _Fact *premise_pattern, _Fact *cause_pattern, uint16_t &write_index) const;

    ConstGuardBuilder(uint64_t period, double constant, uint64_t offset);
public:
    ~ConstGuardBuilder();
};

// fwd: q1=q0*constant.
// bwd: q0=q1/constant.
class MGuardBuilder:
    public ConstGuardBuilder {
public:
    MGuardBuilder(uint64_t period, double constant, uint64_t offset);
    ~MGuardBuilder();

    void build(Code *mdl, _Fact *premise_pattern, _Fact *cause_pattern, uint16_t &write_index) const;
};

// fwd: q1=q0+constant.
// bwd: q0=q1-constant.
class AGuardBuilder:
    public ConstGuardBuilder {
public:
    AGuardBuilder(uint64_t period, double constant, uint64_t offset);
    ~AGuardBuilder();

    void build(Code *mdl, _Fact *premise_pattern, _Fact *cause_pattern, uint16_t &write_index) const;
};
}


#endif