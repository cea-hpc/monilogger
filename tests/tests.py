import unittest
import scihook
import scihook._scihook as sh

class MyOtherEvent:
    pass

class MainTest(unittest.TestCase):

    def setUp(self):
        self.result = 0

    def test_undefined_event(self):
        @scihook.register("MyEvent")
        def test_scihook(ctx):
            print(f"\nReceived MyEvent in context {ctx}\n")
            self.result = self.result + 1

        sh.register_base_event("MyEvent")
        sh.emit_event("MyEvent", sh.SciHookExecutionContext())

        self.assertEqual(self.result, 1)

    def test_class_event(self):
        sh.register_base_event("MyOtherEvent")

        @scihook.register(MyOtherEvent)
        def test_scihook(ctx):
            print(f"\nReceived MyOtherEvent in context {ctx}\n")
            self.result = self.result + 1

        sh.emit_event("MyOtherEvent", sh.SciHookExecutionContext())

        self.assertEqual(self.result, 1)

    def test_composite_event(self):
        sh.register_base_event("MyEvent")
        sh.register_base_event("MyOtherEvent")
        scihook.define_event("MyCompositeEvent", ["MyEvent", "MyOtherEvent"])

        @scihook.register("MyCompositeEvent")
        def test_scihook(ctx):
            print(f"\nReceived MyCompositeEvent in context {ctx}\n")
            self.result = self.result + 1

        sh.emit_event("MyCompositeEvent", sh.SciHookExecutionContext())
        
        self.assertEqual(self.result, 1)

if __name__ == '__main__':
    unittest.main()
