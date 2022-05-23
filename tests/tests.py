import unittest
import scihook
import scihook._scihook as mnlg

class MyOtherEvent:
    pass

class MainTest(unittest.TestCase):

    def setUp(self):
        self.result = 0

    def test_undefined_event(self):
        @scihook.register("MyEvent")
        def test_scihook(ctx):
            self.result = self.result + 1

        mnlg.register_base_event("MyEvent")
        mnlg.emit_event("MyEvent", mnlg.SciHookExecutionContext())

        self.assertEqual(self.result, 1)

    def test_class_event(self):
        mnlg.register_base_event("MyOtherEvent")

        @scihook.register(MyOtherEvent)
        def test_scihook(ctx):
            self.result = self.result + 1

        mnlg.emit_event("MyOtherEvent", mnlg.SciHookExecutionContext())

        self.assertEqual(self.result, 1)

    def test_composite_event(self):
        mnlg.register_base_event("MyEvent")
        mnlg.register_base_event("MyOtherEvent")
        scihook.define_event("MyCompositeEvent", ["MyEvent", "MyOtherEvent"])

        @scihook.register("MyCompositeEvent")
        def test_scihook(ctx):
            self.result = self.result + 1

        mnlg.emit_event("MyCompositeEvent", mnlg.SciHookExecutionContext())
        
        self.assertEqual(self.result, 1)

if __name__ == '__main__':
    unittest.main()
