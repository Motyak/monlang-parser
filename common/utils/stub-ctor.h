#ifndef STUB_CTOR_H
#define STUB_CTOR_H

// dummy type for ctor overload
struct _dummy_stub{};

#define STUB(type) type(_dummy_stub{})

#endif // STUB_CTOR_H
