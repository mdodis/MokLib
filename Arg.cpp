#include "Arg.h"
#include "Containers/Slice.h"
#include "Tape.h"
#include "FileSystem/Extras.h"

static IArg *find_arg(const TArray<IArg*> &args, Str id) {
    for (IArg *arg : args) {
        if (arg->id == id)
            return arg;
    }
    return 0;
}

static bool parse_argument(
	ArgCollection &self,
	const Slice<Str> &args,
	u64 &index);
static bool parse_named_argument(
	ArgCollection &self,
	const Slice<Str> &args,
	u64 &index);
static bool parse_unnamed_argument(
	ArgCollection &self,
	const Slice<Str> &args,
	u64 &index);

bool ArgCollection::parse_args(const Slice<Str> &params) {

	u64 running_index = 0;

	bool error = false;

	while (running_index < params.count) {
		error = !parse_argument(*this, params, running_index);
		if (error)
			break;
	}

	return !error;
}

void ArgCollection::summary() {
	for (IArg *arg : args) {
		if (arg->id == LIT("_")) {
			print(LIT("Unnamed argument: $ ($)\n"), arg->description, arg->type);
		} else {
			print(LIT("-$ ($): $\n"), arg->id, arg->type, arg->description);
		}
	}
}

static bool parse_argument(
	ArgCollection &self,
	const Slice<Str> &args,
	u64 &index)
{
	if (args[index].starts_with(LIT("-"))) {
		return parse_named_argument(self, args, index);
	} else {
		return parse_unnamed_argument(self, args, index);
	}
}

static bool parse_named_argument(
	ArgCollection &self,
	const Slice<Str> &args,
	u64 &index)
{
	Str id = args[index].chop_left(0);
	IArg *arg = find_arg(self.args, id);

	if (!arg) {
		index++;
		return false;
	}

	index++;

	// If no value was specified, try to parse it as a boolean flag
	if (IS_A(arg, TArg<bool>) &&
		(index >= args.count || args[index].starts_with(LIT("-")))) {

		TArg<bool> *barg = (TArg<bool>*)arg;
		barg->current = true;
		return true;
	}

	if (index >= args.count) {
		return false;
	}

	RawTape tape(Raw{args[index].data, args[index].len});
	bool successful = arg->parse(&tape, self.allocator);

	index++;

	return successful;
}

static bool parse_unnamed_argument(
	ArgCollection &self,
	const Slice<Str> &args,
	u64 &index)
{
	IArg *arg = find_arg(self.args, LIT("_"));
	if (!arg)
		return false;


	RawTape tape(Raw{args[index].data, args[index].len});
	index++;
	return arg->parse(&tape, self.allocator);
}

