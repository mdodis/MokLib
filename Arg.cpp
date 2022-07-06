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
		print(LIT("-name ($): $"), arg->type, arg->description);
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

	RawTape tape(Raw{args[index + 1].data, args[index + 1].len});
	bool successful = arg->parse(&tape, self.allocator);

	index += 2;

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

