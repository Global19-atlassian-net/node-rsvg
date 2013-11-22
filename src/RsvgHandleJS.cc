
#include "RsvgHandleJS.h"
#include <node_buffer.h>
#include <cairo-pdf.h>
#include <cmath>

using namespace v8;

Persistent<Function> RsvgHandleJS::constructor;

RsvgHandleJS::RsvgHandleJS(RsvgHandle* const handle) : _handle(handle) {}

RsvgHandleJS::~RsvgHandleJS() {
	g_object_unref(G_OBJECT(_handle));
}

void RsvgHandleJS::Init(Handle<Object> exports) {
	// Prepare constructor template.
	Local<FunctionTemplate> tpl = FunctionTemplate::New(New);
	tpl->SetClassName(String::NewSymbol("RsvgHandle"));
	tpl->InstanceTemplate()->SetInternalFieldCount(1);
	// Add methods to prototype.
	Local<ObjectTemplate> prototype = tpl->PrototypeTemplate();
	prototype->Set(String::NewSymbol("getBaseURI"),
			FunctionTemplate::New(GetBaseURI)->GetFunction());
	prototype->Set(String::NewSymbol("setBaseURI"),
			FunctionTemplate::New(SetBaseURI)->GetFunction());
	prototype->Set(String::NewSymbol("getDPI"),
			FunctionTemplate::New(GetDPI)->GetFunction());
	prototype->Set(String::NewSymbol("setDPI"),
			FunctionTemplate::New(SetDPI)->GetFunction());
	prototype->Set(String::NewSymbol("getDPIX"),
			FunctionTemplate::New(GetDPIX)->GetFunction());
	prototype->Set(String::NewSymbol("setDPIX"),
			FunctionTemplate::New(SetDPIX)->GetFunction());
	prototype->Set(String::NewSymbol("getDPIY"),
			FunctionTemplate::New(GetDPIY)->GetFunction());
	prototype->Set(String::NewSymbol("setDPIY"),
			FunctionTemplate::New(SetDPIY)->GetFunction());
	prototype->Set(String::NewSymbol("getEM"),
			FunctionTemplate::New(GetEM)->GetFunction());
	prototype->Set(String::NewSymbol("getEX"),
			FunctionTemplate::New(GetEX)->GetFunction());
	prototype->Set(String::NewSymbol("getWidth"),
			FunctionTemplate::New(GetWidth)->GetFunction());
	prototype->Set(String::NewSymbol("getHeight"),
			FunctionTemplate::New(GetHeight)->GetFunction());
	prototype->Set(String::NewSymbol("write"),
			FunctionTemplate::New(Write)->GetFunction());
	prototype->Set(String::NewSymbol("close"),
			FunctionTemplate::New(Close)->GetFunction());
	prototype->Set(String::NewSymbol("dimensions"),
			FunctionTemplate::New(Dimensions)->GetFunction());
	prototype->Set(String::NewSymbol("dimensionsOfElement"),
			FunctionTemplate::New(DimensionsOfElement)->GetFunction());
	prototype->Set(String::NewSymbol("positionOfElement"),
			FunctionTemplate::New(PositionOfElement)->GetFunction());
	prototype->Set(String::NewSymbol("hasElement"),
			FunctionTemplate::New(HasElement)->GetFunction());
	prototype->Set(String::NewSymbol("renderRaw"),
			FunctionTemplate::New(RenderRaw)->GetFunction());
	prototype->Set(String::NewSymbol("renderPNG"),
			FunctionTemplate::New(RenderPNG)->GetFunction());
	prototype->Set(String::NewSymbol("renderPDF"),
			FunctionTemplate::New(RenderPDF)->GetFunction());
	prototype->Set(String::NewSymbol("renderSVG"),
			FunctionTemplate::New(RenderSVG)->GetFunction());
	// Export class.
	constructor = Persistent<Function>::New(tpl->GetFunction());
	exports->Set(String::NewSymbol("RsvgHandle"), constructor);
}

Handle<Value> RsvgHandleJS::New(const Arguments& args) {
	HandleScope scope;

	if (args.IsConstructCall()) {
		// Invoked as constructor: `new RsvgHandleJS(...)`
		RsvgHandle* handle;
		if (node::Buffer::HasInstance(args[0])) {
			const guint8* buffer =
				reinterpret_cast<guint8*>(node::Buffer::Data(args[0]));
			gsize length = node::Buffer::Length(args[0]);

			GError* error = NULL;
			handle = rsvg_handle_new_from_data(buffer, length, &error);

			if (error) {
				ThrowException(Exception::Error(String::New(error->message)));
				g_error_free(error);
				return scope.Close(Undefined());
			}
		} else {
			handle = rsvg_handle_new();
		}
		// Error handling.
		if (!handle) {
			ThrowException(Exception::Error(String::New(
				"Unable to create RsvgHandle instance."
			)));
			return scope.Close(Undefined());
		}
		// Create object.
		RsvgHandleJS* obj = new RsvgHandleJS(handle);
		obj->Wrap(args.This());
		return args.This();
	} else {
		// Invoked as plain function `RsvgHandleJS(...)`, turn into construct call.
		const int argc = 1;
		Local<Value> argv[argc] = { args[0] };
		return scope.Close(constructor->NewInstance(argc, argv));
	}
}

Handle<Value> RsvgHandleJS::GetBaseURI(const Arguments& args) {
	HandleScope scope;
	RsvgHandleJS* obj = node::ObjectWrap::Unwrap<RsvgHandleJS>(args.This());
	const char* value = rsvg_handle_get_base_uri(obj->_handle);
	return scope.Close(value ? String::New(value) : Null());
}

Handle<Value> RsvgHandleJS::SetBaseURI(const Arguments& args) {
	HandleScope scope;
	RsvgHandleJS* obj = node::ObjectWrap::Unwrap<RsvgHandleJS>(args.This());
	if (args[0]->IsNull() || args[0]->IsUndefined()) {
		rsvg_handle_set_base_uri(obj->_handle, ""); // TODO: Support null?
	} else {
		v8::String::Utf8Value arg0(args[0]);
		rsvg_handle_set_base_uri(obj->_handle, *arg0);
	}
	return scope.Close(Undefined());
}

Handle<Value> RsvgHandleJS::GetDPI(const Arguments& args) {
	HandleScope scope;
	RsvgHandleJS* obj = node::ObjectWrap::Unwrap<RsvgHandleJS>(args.This());
	gdouble dpiX = 0;
	gdouble dpiY = 0;
	g_object_get(
		G_OBJECT(obj->_handle),
		"dpi-x", &dpiX,
		"dpi-y", &dpiY,
		NULL
	);
	return scope.Close(Number::New(MIN(dpiX, dpiY)));
}

Handle<Value> RsvgHandleJS::SetDPI(const Arguments& args) {
	HandleScope scope;
	RsvgHandleJS* obj = node::ObjectWrap::Unwrap<RsvgHandleJS>(args.This());
	gdouble value = args[0]->NumberValue();
	if (std::isnan(value)) {
		value = 0;
	}
	rsvg_handle_set_dpi(obj->_handle, value);
	return scope.Close(Undefined());
}

Handle<Value> RsvgHandleJS::GetDPIX(const Arguments& args) {
	return GetNumberProperty(args, "dpi-x");
}

Handle<Value> RsvgHandleJS::SetDPIX(const Arguments& args) {
	return SetNumberProperty(args, "dpi-x");
}

Handle<Value> RsvgHandleJS::GetDPIY(const Arguments& args) {
	return GetNumberProperty(args, "dpi-y");
}

Handle<Value> RsvgHandleJS::SetDPIY(const Arguments& args) {
	return SetNumberProperty(args, "dpi-y");
}

Handle<Value> RsvgHandleJS::GetEM(const Arguments& args) {
	return GetNumberProperty(args, "em");
}

Handle<Value> RsvgHandleJS::GetEX(const Arguments& args) {
	return GetNumberProperty(args, "ex");
}

Handle<Value> RsvgHandleJS::GetWidth(const Arguments& args) {
	return GetIntegerProperty(args, "width");
}

Handle<Value> RsvgHandleJS::GetHeight(const Arguments& args) {
	return GetIntegerProperty(args, "height");
}

Handle<Value> RsvgHandleJS::Write(const Arguments& args) {
	HandleScope scope;
	RsvgHandleJS* obj = node::ObjectWrap::Unwrap<RsvgHandleJS>(args.This());
	if (node::Buffer::HasInstance(args[0])) {
		const guchar* buffer =
			reinterpret_cast<guchar*>(node::Buffer::Data(args[0]));
		gsize length = node::Buffer::Length(args[0]);

		GError* error = NULL;
		gboolean success = rsvg_handle_write(obj->_handle, buffer, length, &error);

		if (error) {
			ThrowException(Exception::Error(String::New(error->message)));
			g_error_free(error);
		} else if (!success) {
			ThrowException(Exception::Error(String::New("Failed to write data.")));
		}
	} else {
		ThrowException(Exception::TypeError(String::New("Invalid argument: buffer")));
	}
	return scope.Close(Undefined());
}

Handle<Value> RsvgHandleJS::Close(const Arguments& args) {
	HandleScope scope;
	RsvgHandleJS* obj = node::ObjectWrap::Unwrap<RsvgHandleJS>(args.This());

	GError* error = NULL;
	gboolean success = rsvg_handle_close(obj->_handle, &error);

	if (error) {
		ThrowException(Exception::Error(String::New(error->message)));
		g_error_free(error);
	} else if (!success) {
		ThrowException(Exception::Error(String::New("Failed to close.")));
	}
	return scope.Close(Undefined());
}

Handle<Value> RsvgHandleJS::Dimensions(const Arguments& args) {
	HandleScope scope;
	return scope.Close(Undefined());
}

Handle<Value> RsvgHandleJS::DimensionsOfElement(const Arguments& args) {
	HandleScope scope;
	return scope.Close(Undefined());
}

Handle<Value> RsvgHandleJS::PositionOfElement(const Arguments& args) {
	HandleScope scope;
	return scope.Close(Undefined());
}

Handle<Value> RsvgHandleJS::HasElement(const Arguments& args) {
	HandleScope scope;
	return scope.Close(Undefined());
}

Handle<Value> RsvgHandleJS::RenderRaw(const Arguments& args) {
	HandleScope scope;
	return scope.Close(Undefined());
}

Handle<Value> RsvgHandleJS::RenderPNG(const Arguments& args) {
	HandleScope scope;
	return scope.Close(Undefined());
}

Handle<Value> RsvgHandleJS::RenderPDF(const Arguments& args) {
	HandleScope scope;
	return scope.Close(Undefined());
}

Handle<Value> RsvgHandleJS::RenderSVG(const Arguments& args) {
	HandleScope scope;
	return scope.Close(Undefined());
}

Handle<Value> RsvgHandleJS::GetStringProperty(const Arguments& args, const char* property) {
	HandleScope scope;
	RsvgHandleJS* obj = node::ObjectWrap::Unwrap<RsvgHandleJS>(args.This());
	gchar* value = NULL;
	g_object_get(G_OBJECT(obj->_handle), property, &value, NULL);
	Handle<Value> result(value ? String::New(value) : Null());
	if (value) {
		g_free(value);
	}
	return scope.Close(result);
}

Handle<Value> RsvgHandleJS::SetStringProperty(const Arguments& args, const char* property) {
	HandleScope scope;
	RsvgHandleJS* obj = node::ObjectWrap::Unwrap<RsvgHandleJS>(args.This());
	gchar* value;
	if (args[0]->IsNull() || args[0]->IsUndefined()) {
		value = NULL;
	} else {
		v8::String::Utf8Value arg0(args[0]);
		value = *arg0;
	}
	g_object_set(G_OBJECT(obj->_handle), property, value, NULL);
	return scope.Close(Undefined());
}

Handle<Value> RsvgHandleJS::GetNumberProperty(const Arguments& args, const char* property) {
	HandleScope scope;
	RsvgHandleJS* obj = node::ObjectWrap::Unwrap<RsvgHandleJS>(args.This());
	gdouble value = 0;
	g_object_get(G_OBJECT(obj->_handle), property, &value, NULL);
	return scope.Close(Number::New(value));
}

Handle<Value> RsvgHandleJS::SetNumberProperty(const Arguments& args, const char* property) {
	HandleScope scope;
	RsvgHandleJS* obj = node::ObjectWrap::Unwrap<RsvgHandleJS>(args.This());
	gdouble value = args[0]->NumberValue();
	if (std::isnan(value)) {
		value = 0;
	}
	g_object_set(G_OBJECT(obj->_handle), property, value, NULL);
	return scope.Close(Undefined());
}

Handle<Value> RsvgHandleJS::GetIntegerProperty(const Arguments& args, const char* property) {
	HandleScope scope;
	RsvgHandleJS* obj = node::ObjectWrap::Unwrap<RsvgHandleJS>(args.This());
	gint value = 0;
	g_object_get(G_OBJECT(obj->_handle), property, &value, NULL);
	return scope.Close(Integer::New(value));
}

Handle<Value> RsvgHandleJS::SetIntegerProperty(const Arguments& args, const char* property) {
	HandleScope scope;
	RsvgHandleJS* obj = node::ObjectWrap::Unwrap<RsvgHandleJS>(args.This());
	gint value = args[0]->Int32Value();
	g_object_set(G_OBJECT(obj->_handle), property, value, NULL);
	return scope.Close(Undefined());
}